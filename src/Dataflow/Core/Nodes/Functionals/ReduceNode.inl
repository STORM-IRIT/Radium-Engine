#pragma once
#include <Dataflow/Core/Nodes/Functionals/ReduceNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName ) :
    ReduceNode(
        instanceName,
        getTypename(),
        []( const v_t& a, const v_t& ) -> v_t { return a; },
        v_t {} ) {}

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName,
                                     ReduceOperator op,
                                     v_t initialValue ) :
    ReduceNode( instanceName, getTypename(), op, initialValue ) {}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::setOperator( ReduceOperator op, v_t initialValue ) {
    m_operator = op;
    m_init     = initialValue;
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::init() {
    Node::init();
    m_result = m_init;
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::execute() {
    auto f   = m_portF->isLinked() ? m_portF->getData() : m_operator;
    auto iv  = m_portInit->isLinked() ? m_portInit->getData() : m_init;
    m_result = iv;
    // The following test will always be true if the node was integrated in a compiled graph
    if ( m_portIn->isLinked() ) {
        const auto& inData = m_portIn->getData();
        m_result           = std::accumulate( inData.begin(), inData.end(), iv, f );
#ifdef GRAPH_CALL_TRACE
        std::cout << "\e[36m\e[1mMReduceNode \e[0m \"" << m_instanceName << "\": execute, from "
                  << input->getData().size() << " " << getTypename() << "." << std::endl;
#endif
    }
}

template <typename coll_t, typename v_t>
const std::string& ReduceNode<coll_t, v_t>::getTypename() {
    static std::string demangledName =
        std::string { "Reduce<" } + Ra::Dataflow::Core::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName,
                                     const std::string& typeName,
                                     ReduceOperator op,
                                     v_t initialValue ) :
    Node( instanceName, typeName ), m_operator( op ), m_init( initialValue ) {

    addInput( m_portIn );
    m_portIn->mustBeLinked();
    addInput( m_portF );
    addInput( m_portInit );
    addOutput( m_portOut, &m_result );
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] =
        std::string { "Reduce operator could not be serialized for " } + getTypeName();
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
