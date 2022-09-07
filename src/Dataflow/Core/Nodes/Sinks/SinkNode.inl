#pragma once
#include <Core/Utils/TypesUtils.hpp>

#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sinks {

template <typename T>
SinkNode<T>::SinkNode( const std::string& name ) : Node( name, getTypename() ) {
    auto portIn = new PortIn<T>( "from", this );
    portIn->mustBeLinked();
    addInput( portIn );
}

template <typename T>
void SinkNode<T>::execute() {
    auto input     = static_cast<PortIn<T>*>( m_inputs[0].get() );
    auto interface = static_cast<PortOut<T>*>( m_interface[0] );
    if ( input->isLinked() ) {
        // If interface is linked, do not copy the data, just transmit the pointer
        interface->setData( &( input->getData() ) );
    }
    else {
        m_data = input->getData();
    }
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[33m\e[1m" << getTypename() << "\e[0m \"" << getInstanceName() << "\": execute."
              << std::endl;
#endif
}

template <typename T>
T SinkNode<T>::getData() const {
    return m_data;
}

template <typename T>
const T& SinkNode<T>::getDataByRef() const {
    return m_data;
}

template <typename T>
const std::string SinkNode<T>::getTypename() {
    return std::string { "Sink::" } + Ra::Core::Utils::demangleType<T>();
}

template <typename T>
void SinkNode<T>::toJsonInternal( nlohmann::json& data ) const {}

template <typename T>
void SinkNode<T>::fromJsonInternal( const nlohmann::json& data ) {}

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra
