#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Enumerator.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/BijectiveAssociation.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Singleton.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {

class RA_DATAFLOW_CORE_API PortFactory
{
    RA_SINGLETON_INTERFACE( PortFactory );

  public:
    using PortInCtorFunctor  = std::function<Node::PortBaseInPtr( Node*, const std::string& )>;
    using PortOutCtorFunctor = std::function<Node::PortBaseOutPtr( Node*, const std::string& )>;
    using PortOutSetter      = std::function<void( PortBaseOut*, std::any )>;
    using PortInGetter       = std::function<std::any( PortBaseIn* )>;

    Node::PortBaseInPtr
    make_input_port( Node* node, const std::string& name, std::type_index type ) {
        return m_input_ctor.at( type )( node, name );
    }
    Node::PortBaseOutPtr
    make_output_port( Node* node, const std::string& name, std::type_index type ) {
        return m_output_ctor.at( type )( node, name );
    }

    Node::PortBaseInPtr
    make_input_port_from_name( Node* node, const std::string& name, std::string type ) {
        return make_input_port( node, name, m_type_to_string.key( type ) );
    }
    Node::PortBaseOutPtr
    make_output_port_from_name( Node* node, const std::string& name, std::string type ) {
        return make_output_port( node, name, m_type_to_string.key( type ) );
    }

    PortOutSetter output_setter( std::type_index type ) { return m_output_setter[type]; }
    PortInGetter input_getter( std::type_index type ) { return m_input_getter[type]; }

    template <typename T>
    void add_port_type() {
        auto type          = std::type_index( typeid( T ) );
        m_input_ctor[type] = []( Node* node, const std::string& name ) {
            return std::make_shared<PortIn<T>>( node, name );
        };

        m_output_ctor[type] = []( Node* node, const std::string& name ) {
            return std::make_shared<PortOut<T>>( node, name );
        };

        m_input_getter[type] = []( PortBaseIn* port ) -> std::any {
            auto casted = dynamic_cast<PortIn<T>*>( port );
            return &( casted->getData() );
        };
        m_output_setter[type] = []( PortBaseOut* port, std::any any ) {
            T* data     = std::any_cast<T*>( any );
            auto casted = dynamic_cast<PortOut<T>*>( port );
            casted->setData( data );
        };

        m_type_to_string.insert( type, Ra::Core::Utils::simplifiedDemangledType( type ) );
    }

  private:
    PortFactory() {
        using namespace Ra::Core;
        add_port_type<Scalar>();
        add_port_type<int>();
        add_port_type<unsigned int>();
        add_port_type<Utils::Color>();
        add_port_type<Vector2>();
        add_port_type<Vector3>();
        add_port_type<Vector4>();
    }

    std::unordered_map<std::type_index, PortInCtorFunctor> m_input_ctor;
    std::unordered_map<std::type_index, PortInGetter> m_input_getter;
    std::unordered_map<std::type_index, PortOutCtorFunctor> m_output_ctor;
    std::unordered_map<std::type_index, PortOutSetter> m_output_setter;

    Ra::Core::Utils::BijectiveAssociation<std::type_index, std::string> m_type_to_string;
};

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
        auto factory    = PortFactory::getInstance();
        auto in_name    = find_available_name( "in", port->getName() );
        auto in         = factory->make_input_port( this, in_name, port->getType() );
        auto out_name   = find_available_name( "out", port->getName() );
        auto out        = factory->make_output_port( this, out_name, port->getType() );
        auto input_idx  = addInput( in );
        auto output_idx = addOutput( out );
        return std::make_tuple( input_idx, output_idx, in, out );
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
        port->connect( out.get() );
        return input_idx;
    }
};

class RA_DATAFLOW_CORE_API GraphOutputNode : public GraphNode
{
    BASIC_NODE_INIT( GraphOutputNode, GraphNode ) {}

  public:
    PortIndex add_input_port( PortBaseOutRawPtr port ) {
        auto [input_idx, output_idx, in, out] = add_ports( port );

        in->connect( port );
        return output_idx;
    }
};
} // namespace Core
} // namespace Dataflow
} // namespace Ra
