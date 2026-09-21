#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Core/Utils/BijectiveAssociation.hpp>
#include <Core/Utils/Singleton.hpp>
#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class RA_DATAFLOW_CORE_API PortFactory
{
    RA_SINGLETON_INTERFACE( PortFactory );

  public:
    using PortInCtorFunctor  = std::function<PortBaseInPtr( Node*, const std::string& )>;
    using PortOutCtorFunctor = std::function<PortBaseOutPtr( Node*, const std::string& )>;
    using PortOutSetter      = std::function<void( PortBaseOut*, std::any )>;
    using PortInGetter       = std::function<std::any( PortBaseIn* )>;

    PortBaseInPtr make_input_port( Node* node, const std::string& name, std::string type ) {
        if ( auto itr = m_input_ctor.find( type ); itr != m_input_ctor.end() ) {
            return itr->second( node, name );
        }
        LOG( Ra::Core::Utils::logERROR )
            << "input ctor type not found " << Ra::Core::Utils::simplifiedDemangledType( type );
        return {};
    }
    PortBaseOutPtr make_output_port( Node* node, const std::string& name, std::string type ) {
        if ( auto itr = m_output_ctor.find( type ); itr != m_output_ctor.end() ) {
            return itr->second( node, name );
        }
        LOG( Ra::Core::Utils::logERROR )
            << "output ctor type not found " << Ra::Core::Utils::simplifiedDemangledType( type );
        return {};
    }

    PortOutSetter output_setter( std::string type ) { return m_output_setter.at( type ); }
    PortInGetter input_getter( std::string type ) { return m_input_getter.at( type ); }

    template <typename T>
    void add_port_type() {

        auto type = Ra::Core::Utils::simplifiedDemangledType<T>();
        if ( !m_input_ctor.contains( type ) ) {
            m_input_ctor[type] = []( Node* node, const std::string& name ) {
                return std::make_shared<PortIn<T>>( node, name );
            };

            m_output_ctor[type] = []( Node* node, const std::string& name ) {
                return std::make_shared<PortOut<T>>( node, name );
            };

            m_input_getter[type] = []( PortBaseIn* port ) -> std::any {
                auto casted = dynamic_cast<PortIn<T>*>( port );
                return &( casted->data() );
            };
            m_output_setter[type] = []( PortBaseOut* port, std::any any ) {
                T* data     = std::any_cast<T*>( any );
                auto casted = dynamic_cast<PortOut<T>*>( port );
                casted->set_data( data );
            };
        }
    }

  private:
    PortFactory() {
        // add_port_type is done in port ctor.
        // might be needed here for serialization (if add_port_type before create node with these
        // port.
        using namespace Ra::Core;
        add_port_type<Scalar>();
        add_port_type<int>();
        add_port_type<unsigned int>();
        add_port_type<Utils::Color>();
        add_port_type<Vector2>();
        add_port_type<Vector3>();
        add_port_type<Vector4>();
        add_port_type<std::function<float( const float& )>>();
    }

    std::unordered_map<std::string, PortInCtorFunctor> m_input_ctor;
    std::unordered_map<std::string, PortInGetter> m_input_getter;
    std::unordered_map<std::string, PortOutCtorFunctor> m_output_ctor;
    std::unordered_map<std::string, PortOutSetter> m_output_setter;
};
template <typename T>
void add_port_type() {
    PortFactory::getInstance()->add_port_type<T>();
}
} // namespace Core
} // namespace Dataflow
} // namespace Ra
