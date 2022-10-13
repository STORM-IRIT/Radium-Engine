#pragma once
#include <Dataflow/Core/Nodes/Functionals/TransformNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

template <typename coll_t, typename v_t>
TransformNode<coll_t, v_t>::TransformNode( const std::string& instanceName ) :
    TransformNode( instanceName, getTypename(), []( v_t ) { return v_t {}; } ) {}

template <typename coll_t, typename v_t>
TransformNode<coll_t, v_t>::TransformNode( const std::string& instanceName, TransformOperator op ) :
    TransformNode( instanceName, getTypename(), op ) {}

template <typename coll_t, typename v_t>
void TransformNode<coll_t, v_t>::setOperator( TransformOperator op ) {
    m_operator = op;
}

template <typename coll_t, typename v_t>
void TransformNode<coll_t, v_t>::init() {
    Node::init();
    m_elements.clear();
}

template <typename coll_t, typename v_t>
void TransformNode<coll_t, v_t>::execute() {
    auto f = m_portOperator->isLinked() ? m_portOperator->getData() : m_operator;
    // The following test will always be true if the node was integrated in a compiled graph
    if ( m_portIn->isLinked() ) {
        const auto& inData = m_portIn->getData();
        m_elements.clear();
        // m_elements.reserve( inData.size() ); // --> this is not a requirement of
        // SequenceContainer
        std::transform( inData.begin(), inData.end(), std::back_inserter( m_elements ), f );
#ifdef GRAPH_CALL_TRACE
        std::cout << "\e[36m\e[1mMapNode \e[0m \"" << m_instanceName << "\": execute, from "
                  << input->getData().size() << " to " << m_elements.size() << " "
                  << typeid( T ).name() << "." << std::endl;
#endif
    }
}

template <typename coll_t, typename v_t>
const std::string& TransformNode<coll_t, v_t>::getTypename() {
    static std::string demangledName =
        std::string { "Transform<" } + Ra::Dataflow::Core::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
TransformNode<coll_t, v_t>::TransformNode( const std::string& instanceName,
                                           const std::string& typeName,
                                           TransformOperator op ) :
    Node( instanceName, typeName ), m_operator( op ) {
    addInput( m_portIn );
    m_portIn->mustBeLinked();
    addInput( m_portOperator );
    addOutput( m_portOut, &m_elements );
}

template <typename coll_t, typename v_t>
void TransformNode<coll_t, v_t>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] =
        std::string { "Transform operator could not be serialized for " } + getTypeName();
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename coll_t, typename v_t>
void TransformNode<coll_t, v_t>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
