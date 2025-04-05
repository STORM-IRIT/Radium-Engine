#include <Dataflow/RaDataflow.hpp>

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

    PortBaseInPtr make_input_port( Node* node, const std::string& name, std::type_index type ) {
        if ( auto itr = m_input_ctor.find( type ); itr != m_input_ctor.end() ) {
            return itr->second( node, name );
        }
        LOG( Ra::Core::Utils::logERROR )
            << "input ctor type not found " << Ra::Core::Utils::simplifiedDemangledType( type );
        return {};
    }
    PortBaseOutPtr make_output_port( Node* node, const std::string& name, std::type_index type ) {
        if ( auto itr = m_output_ctor.find( type ); itr != m_output_ctor.end() ) {
            return itr->second( node, name );
        }
        LOG( Ra::Core::Utils::logERROR )
            << "output ctor type not found " << Ra::Core::Utils::simplifiedDemangledType( type );
        return {};
    }

    PortBaseInPtr
    make_input_port_from_name( Node* node, const std::string& name, std::string type ) {
        return make_input_port( node, name, m_type_to_string.key( type ) );
    }
    PortBaseOutPtr
    make_output_port_from_name( Node* node, const std::string& name, std::string type ) {
        return make_output_port( node, name, m_type_to_string.key( type ) );
    }

    PortOutSetter output_setter( std::type_index type ) { return m_output_setter[type]; }
    PortInGetter input_getter( std::type_index type ) { return m_input_getter[type]; }

    template <typename T>
    void add_port_type() {

        auto type = std::type_index( typeid( T ) );
        if ( !m_type_to_string.valueIfExists( type ) ) {
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

    std::unordered_map<std::type_index, PortInCtorFunctor> m_input_ctor;
    std::unordered_map<std::type_index, PortInGetter> m_input_getter;
    std::unordered_map<std::type_index, PortOutCtorFunctor> m_output_ctor;
    std::unordered_map<std::type_index, PortOutSetter> m_output_setter;

    Ra::Core::Utils::BijectiveAssociation<std::type_index, std::string> m_type_to_string;
};
template <typename T>
void add_port_type() {
    PortFactory::getInstance()->add_port_type<T>();
}
} // namespace Core
} // namespace Dataflow
} // namespace Ra
