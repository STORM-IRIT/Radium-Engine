#pragma once
#include <Core/Utils/TypesUtils.hpp>
#include <Dataflow/Core/Nodes/Sources/FunctionSource.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

template <class R, class... Args>
FunctionSourceNode<R, Args...>::FunctionSourceNode( const std::string& instanceName,
                                                    const std::string& typeName ) :
    Node( instanceName, typeName ) {
    addOutput( m_portOut, m_data );
}

template <class R, class... Args>
void FunctionSourceNode<R, Args...>::execute() {
    auto interface = static_cast<PortIn<function_type>*>( m_interface[0] );
    if ( interface->isLinked() ) { m_data = &( interface->getData() ); }
    else {
        m_data = &m_localData;
    }
    m_portOut->setData( m_data );
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[34m\e[1mFunctionSourceNode\e[0m \"" << m_instanceName << "\": execute."
              << std::endl;
#endif
}

template <class R, class... Args>
void FunctionSourceNode<R, Args...>::setData( function_type* data ) {
    m_localData = *data;
    m_data      = &m_localData;
    m_portOut->setData( m_data );
}

template <class R, class... Args>
typename FunctionSourceNode<R, Args...>::function_type*
FunctionSourceNode<R, Args...>::getData() const {
    return m_data;
}

template <class R, class... Args>
const std::string& FunctionSourceNode<R, Args...>::getTypename() {
    static std::string demangledTypeName =
        std::string { "Source<" } + Ra::Dataflow::Core::simplifiedDemangledType<function_type>() +
        ">";
    return demangledTypeName;
}

template <class R, class... Args>
void FunctionSourceNode<R, Args...>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] = std::string( "Unable to save data when serializing a FunctionSourceNode<" ) +
                      Ra::Dataflow::Core::simplifiedDemangledType<function_type>() + ">.";
    LOG( Ra::Core::Utils::logDEBUG )
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <class R, class... Args>
void FunctionSourceNode<R, Args...>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logDEBUG )
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
