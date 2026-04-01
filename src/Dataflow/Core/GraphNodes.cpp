#include <Dataflow/Core/GraphNodes.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {

template <typename T>
void make_port_helper(
    Node* n,
    const nlohmann::json& ports,
    std::map<size_t, T>& port_map,
    std::function<T( Node* node, const std::string& name, std::string type )> ctor ) {
    for ( const auto& port : ports ) {
        size_t index     = port["port_index"];
        std::string type = port["type"];
        std::string name = port["name"];
        port_map[index]  = ctor( n, name, type );
    }
}

// ---------------------------
// GraphNode
// ---------------------------

const std::string& GraphNode::node_typename() {
    static std ::string demangledName { "GraphNode" };
    return demangledName;
}
bool GraphNode::execute() {
    CORE_ASSERT( m_inputs.size() == m_outputs.size(), "GraphNode input and output size differ" );

    for ( size_t i = 0; i < m_inputs.size(); ++i ) {
        auto factory       = PortFactory::getInstance();
        auto output_setter = factory->output_setter( m_outputs[i]->type() );
        auto input_getter  = factory->input_getter( m_inputs[i]->type() );
        output_setter( m_outputs[i].get(), input_getter( m_inputs[i].get() ) );
    }
    return true;
}

void GraphNode::remove_unlinked_ports() {
    CORE_ASSERT( m_inputs.size() == m_outputs.size(), "GraphNode input and output size differ" );

    int last_index = m_inputs.size();
    for ( int i = 0; i < last_index; ++i ) {
        if ( !m_inputs[i]->is_linked() && m_outputs[i]->link_count() == 0 ) {
            std::swap( m_inputs[i], m_inputs[last_index - 1] );
            std::swap( m_outputs[i], m_outputs[last_index - 1] );
            --last_index;
            --i;
        }
    }
    m_inputs.erase( m_inputs.begin() + last_index, m_inputs.end() );
    m_outputs.erase( m_outputs.begin() + last_index, m_outputs.end() );
}

auto GraphNode::find_available_name( const std::string& type, const std::string& name )
    -> std::string {
    int suffix           = 1;
    std::string new_name = name;
    while ( port_by_name( type, new_name ).first.isValid() ) {
        new_name = name + "_" + std::to_string( suffix++ );
    }
    return new_name;
}

auto GraphNode::add_ports( PortBaseRawPtr port )
    -> std::tuple<PortIndex, PortIndex, PortBaseInPtr, PortBaseOutPtr> {
    auto factory  = PortFactory::getInstance();
    auto in_name  = find_available_name( "in", port->name() );
    auto in       = factory->make_input_port( this, in_name, port->type() );
    auto out_name = find_available_name( "out", port->name() );
    auto out      = factory->make_output_port( this, out_name, port->type() );
    if ( in && out ) {
        auto input_idx  = add_input( in );
        auto output_idx = add_output( out );
        return std::make_tuple( input_idx, output_idx, in, out );
    }
    return std::make_tuple( PortIndex {}, PortIndex {}, in, out );
}
bool GraphNode::fromJsonInternal( const nlohmann::json& data ) {
    auto factory = PortFactory::getInstance();
    std::map<size_t, PortBaseInPtr> inputs;
    std::map<size_t, PortBaseOutPtr> outputs;
    using namespace std::placeholders;
    if ( const auto& ports = data.find( "inputs" ); ports != data.end() ) {
        auto ctor = std::bind( &PortFactory::make_input_port_from_name, factory, _1, _2, _3 );
        make_port_helper<PortBaseInPtr>( this, *ports, inputs, ctor );
    }
    if ( const auto& ports = data.find( "outputs" ); ports != data.end() ) {
        auto ctor = std::bind( &PortFactory::make_output_port_from_name, factory, _1, _2, _3 );
        make_port_helper<PortBaseOutPtr>( this, *ports, outputs, ctor );
    }

    m_inputs.clear();
    m_outputs.clear();
    for ( const auto& [key, value] : inputs ) {
        assert( m_inputs.size() == key );
        m_inputs.push_back( value );
    }
    for ( const auto& [key, value] : outputs ) {
        assert( m_outputs.size() == key );
        m_outputs.push_back( value );
    }
    CORE_ASSERT( m_inputs.size() == m_outputs.size(),
                 "json do not contains same number of inputs and outputs for GraphNode" );

    return true;
}

// ---------------------------
// GraphInputNode
// ---------------------------

const std::string& GraphInputNode::node_typename() {
    static std::string demangledName { "GraphInputNode" };
    //    std::cerr << " node typename\n";
    return demangledName;
}
auto GraphInputNode::add_output_port( PortBaseInRawPtr port ) -> PortIndex {
    auto [input_idx, output_idx, in, out] = add_ports( port );
    if ( in && out ) port->connect( out.get() );
    return input_idx;
}

// ---------------------------
// GraphOutputNode
// ---------------------------

const std::string& GraphOutputNode::node_typename() {
    static std ::string demangledName { "GraphOutputNode" };
    return demangledName;
}

auto GraphOutputNode::add_input_port( PortBaseOutRawPtr port ) -> PortIndex {
    auto [input_idx, output_idx, in, out] = add_ports( port );
    if ( in && out ) in->connect( port );
    return output_idx;
}
} // namespace Core
} // namespace Dataflow
} // namespace Ra
