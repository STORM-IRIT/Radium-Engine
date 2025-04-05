#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Enumerator.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>

#include <Core/Types.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {

#define BASIC_NODE_INIT( TYPE, BASE )                                               \
  public:                                                                           \
    explicit TYPE( const std::string& name ) : TYPE( name, TYPE::getTypename() ) {} \
    static const std::string& getTypename() {                                       \
        static std::string demangledName = #TYPE;                                   \
        return demangledName;                                                       \
    }                                                                               \
    TYPE( const std::string& instanceName, const std::string& typeName ) :          \
        BASE( instanceName, typeName )

class RA_DATAFLOW_CORE_API GraphNode : public Node
{
    BASIC_NODE_INIT( GraphNode, Node ) {}

  public:
    bool execute() override {
        CORE_ASSERT( m_inputs.size() == m_outputs.size(),
                     "GraphNode input and output size differ" );

        for ( size_t i = 0; i < m_inputs.size(); ++i ) {
            auto factory       = PortFactory::getInstance();
            auto output_setter = factory->output_setter( m_outputs[i]->getType() );
            auto input_getter  = factory->input_getter( m_inputs[i]->getType() );
            output_setter( m_outputs[i].get(), input_getter( m_inputs[i].get() ) );
        }
        return true;
    }

    void remove_unlinked_ports() {
        CORE_ASSERT( m_inputs.size() == m_outputs.size(),
                     "GraphNode input and output size differ" );

        int last_index = m_inputs.size();
        for ( int i = 0; i < last_index; ++i ) {
            if ( !m_inputs[i]->isLinked() && m_outputs[i]->getLinkCount() == 0 ) {
                std::swap( m_inputs[i], m_inputs[last_index - 1] );
                std::swap( m_outputs[i], m_outputs[last_index - 1] );
                --last_index;
                --i;
            }
        }
        m_inputs.erase( m_inputs.begin() + last_index, m_inputs.end() );
        m_outputs.erase( m_outputs.begin() + last_index, m_outputs.end() );
    }

    void set_graph( Node* node ) { m_graph = node; }
    Node* graph() const { return m_graph; }

  protected:
    auto add_ports( PortBaseRawPtr port ) {
        auto factory  = PortFactory::getInstance();
        auto in_name  = find_available_name( "in", port->getName() );
        auto in       = factory->make_input_port( this, in_name, port->getType() );
        auto out_name = find_available_name( "out", port->getName() );
        auto out      = factory->make_output_port( this, out_name, port->getType() );
        if ( in && out ) {
            auto input_idx  = addInput( in );
            auto output_idx = addOutput( out );
            return std::make_tuple( input_idx, output_idx, in, out );
        }
        return std::make_tuple( PortIndex {}, PortIndex {}, in, out );
    }

    auto find_available_name( const std::string& type, const std::string& name ) -> std::string {
        int suffix           = 1;
        std::string new_name = name;
        while ( getPortByName( type, new_name ).first.isValid() ) {
            new_name = name + "_" + std::to_string( suffix++ );
        }
        return new_name;
    }

    template <typename T>
    void make_port_helper(
        const nlohmann::json& ports,
        std::map<size_t, T>& port_map,
        std::function<T( Node* node, const std::string& name, std::string type )> ctor ) {
        for ( const auto& port : ports ) {
            size_t index     = port["port_index"];
            std::string type = port["type"];
            std::string name = port["name"];
            port_map[index]  = ctor( this, name, type );
        }
    }

    bool fromJsonInternal( const nlohmann::json& data ) override {
        auto factory = PortFactory::getInstance();
        std::map<size_t, PortBaseInPtr> inputs;
        std::map<size_t, PortBaseOutPtr> outputs;
        using namespace std::placeholders;
        if ( const auto& ports = data.find( "inputs" ); ports != data.end() ) {
            auto ctor = std::bind( &PortFactory::make_input_port_from_name, factory, _1, _2, _3 );
            make_port_helper<PortBaseInPtr>( *ports, inputs, ctor );
        }
        if ( const auto& ports = data.find( "outputs" ); ports != data.end() ) {
            auto ctor = std::bind( &PortFactory::make_output_port_from_name, factory, _1, _2, _3 );
            make_port_helper<PortBaseOutPtr>( *ports, outputs, ctor );
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

  private:
    Node* m_graph { nullptr };
};

class RA_DATAFLOW_CORE_API GraphInputNode : public GraphNode
{
    BASIC_NODE_INIT( GraphInputNode, GraphNode ) {}

  public:
    PortIndex add_output_port( PortBaseInRawPtr port ) {
        auto [input_idx, output_idx, in, out] = add_ports( port );
        if ( in && out ) port->connect( out.get() );
        return input_idx;
    }
};

class RA_DATAFLOW_CORE_API GraphOutputNode : public GraphNode
{
    BASIC_NODE_INIT( GraphOutputNode, GraphNode ) {}

  public:
    PortIndex add_input_port( PortBaseOutRawPtr port ) {
        auto [input_idx, output_idx, in, out] = add_ports( port );
        if ( in && out ) in->connect( port );
        return output_idx;
    }
};
} // namespace Core
} // namespace Dataflow
} // namespace Ra
