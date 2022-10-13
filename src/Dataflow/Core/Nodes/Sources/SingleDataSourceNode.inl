#pragma once
#include <Core/Utils/TypesUtils.hpp>
#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

template <typename T>
SingleDataSourceNode<T>::SingleDataSourceNode( const std::string& instanceName,
                                               const std::string& typeName ) :
    Node( instanceName, typeName ) {
    addOutput( m_portOut, m_data );
}

template <typename T>
void SingleDataSourceNode<T>::execute() {
    // interfaces ports are at the same index as output ports
    auto interfacePort = static_cast<PortIn<T>*>( m_interface[0] );
    if ( interfacePort->isLinked() ) {
        // use external storage to deliver data
        m_data = &( interfacePort->getData() );
    }
    else {
        // use local storage to deliver data
        m_data = &m_localData;
    }
    m_portOut->setData( m_data );
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[34m\e[1mSingleDataSourceNode\e[0m \"" << m_instanceName << "\": execute."
              << std::endl;
#endif
}

template <typename T>
void SingleDataSourceNode<T>::setData( T* data ) {
    /// \warning this will copy data into local storage
    m_localData = *data;
#if 0
    m_data = &m_localData;
    m_portOut->setData( m_data );
#endif
}

template <typename T>
void SingleDataSourceNode<T>::setData( T& data ) {
    m_localData = data;
#if 0
    m_data = &m_localData;
    m_portOut->setData( m_data );
#endif
}

template <typename T>
T* SingleDataSourceNode<T>::getData() const {
    return m_data;
}

template <typename T>
void SingleDataSourceNode<T>::setEditable( const std::string& name ) {
    Node::addEditableParameter( new EditableParameter( name, m_localData ) );
}

template <typename T>
void SingleDataSourceNode<T>::removeEditable( const std::string& name ) {
    Node::removeEditableParameter( name );
}

template <typename T>
const std::string& SingleDataSourceNode<T>::getTypename() {
    static std::string demangledTypeName =
        std::string { "Source<" } + Ra::Dataflow::Core::simplifiedDemangledType<T>() + ">";
    return demangledTypeName;
}

template <typename T>
void SingleDataSourceNode<T>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] =
        std::string( "Unable to save data when serializing a SingleDataSourceNode<" ) +
        Ra::Dataflow::Core::simplifiedDemangledType<T>() + ">.";
    LOG( Ra::Core::Utils::logDEBUG )
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename T>
void SingleDataSourceNode<T>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logDEBUG )
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
