#pragma once
#include <Core/Utils/TypesUtils.hpp>

#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sinks {

template <typename T>
SinkNode<T>::SinkNode( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ) {

    m_portIn->mustBeLinked();
    addInput( m_portIn );
}

template <typename T>
void SinkNode<T>::init() {
    // this should be done only once (or when the address of local data changes)
    // What if interfaces were not created before init (e.g. call of init on a node not added to a
    // graph) Todo : assert on the existence of the interface
    auto interfacePort = static_cast<PortOut<T>*>( m_interface[0] );
    interfacePort->setData( &m_data );
    Node::init();
}

template <typename T>
bool SinkNode<T>::execute() {
    m_data = m_portIn->getData();
    return true;
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
const std::string& SinkNode<T>::getTypename() {
    static std::string demangledName =
        std::string { "Sink<" } + Ra::Dataflow::Core::simplifiedDemangledType<T>() + ">";
    return demangledName;
}

template <typename T>
void SinkNode<T>::toJsonInternal( nlohmann::json& ) const {}

template <typename T>
bool SinkNode<T>::fromJsonInternal( const nlohmann::json& ) {
    return true;
}

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra
