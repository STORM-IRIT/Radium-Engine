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
    m_data    = &m_localData;
    m_portOut = new PortOut<T>( "to", this );
    addOutput( m_portOut, m_data );
}

template <typename T>
void SingleDataSourceNode<T>::execute() {
    auto interface = static_cast<PortIn<T>*>( m_interface[0] );
    if ( interface->isLinked() ) { m_data = &( interface->getData() ); }
    else {
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
    if ( m_data == &m_localData ) {
        // TODO : do we need to copy the data ?
        std::cerr << "SingleDataSourceNode<T>::setData : overriding the local data !!!\n";
        m_localData = *data;
    }
    else {
        m_data = data;
    }
    m_portOut->setData( m_data );
}

template <typename T>
void SingleDataSourceNode<T>::setData( T& data ) {
    // TODO : assert m_data == &m_localData ???
    m_localData = data;
    m_portOut->setData( m_data );
}

template <typename T>
T* SingleDataSourceNode<T>::getData() const {
    return m_data;
}

template <typename T>
void SingleDataSourceNode<T>::setEditable( const std::string name ) {
    Node::addEditableParameter<T>( new EditableParameter( name, m_localData ) );
}

template <typename T>
void SingleDataSourceNode<T>::removeEditable( const std::string& name ) {
    Node::removeEditableParameter<T>( name );
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
