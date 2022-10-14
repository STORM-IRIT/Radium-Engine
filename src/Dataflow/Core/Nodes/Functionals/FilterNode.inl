#pragma once
#include <Dataflow/Core/Nodes/Functionals/FilterNode.hpp>

#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName ) :
    FilterNode( instanceName, getTypename(), []( v_t ) { return true; } ) {}

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName, UnaryPredicate predicate ) :
    FilterNode( instanceName, getTypename(), predicate ) {}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::setFilterFunction( UnaryPredicate predicate ) {
    m_predicate = predicate;
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::init() {
    Node::init();
    m_elements.clear();
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::execute() {
    auto f = m_portPredicate->isLinked() ? m_portPredicate->getData() : m_predicate;
    // The following test will always be true if the node was integrated in a compiled graph
    if ( m_portIn->isLinked() ) {
        const auto& inData = m_portIn->getData();
        m_elements.clear();
        // m_elements.reserve( inData.size() ); // --> this is not a requirement of
        // SequenceContainer
        std::copy_if( inData.begin(), inData.end(), std::back_inserter( m_elements ), f );
    }
}

template <typename coll_t, typename v_t>
const std::string& FilterNode<coll_t, v_t>::getTypename() {
    static std::string demangledName =
        std::string { "Filter<" } + Ra::Dataflow::Core::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName,
                                     const std::string& typeName,
                                     UnaryPredicate filterFunction ) :
    Node( instanceName, typeName ), m_predicate( filterFunction ) {

    addInput( m_portIn );
    m_portIn->mustBeLinked();
    addInput( m_portPredicate );
    addOutput( m_portOut, &m_elements );
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] =
        std::string { "Filtering function could not be serialized for " } + getTypeName();
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename coll_t, typename v_t>
bool FilterNode<coll_t, v_t>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
    return true;
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
