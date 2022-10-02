#pragma once
#include <Dataflow/Core/Nodes/Filters/FilterNode.hpp>

#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Filters {

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName ) :
    FilterNode( instanceName, getTypename(), []( v_t ) { return true; } ) {}

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName,
                                     UnaryPredicate filterFunction ) :
    FilterNode( instanceName, getTypename(), filterFunction ) {}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::setFilterFunction( UnaryPredicate filterFunction ) {
    m_filterFunction = filterFunction;
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::init() {
    Node::init();
    m_elements.clear();
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::execute() {
    auto input = dynamic_cast<PortIn<coll_t>*>( m_inputs[0].get() );
    if ( input->isLinked() ) {
        const auto& inData = input->getData();
        m_elements.clear();
        // m_elements.reserve( inData.size() ); // --> this is not a requirement of
        // SequenceContainer
        std::copy_if(
            inData.begin(), inData.end(), std::back_inserter( m_elements ), m_filterFunction );
#ifdef GRAPH_CALL_TRACE
        std::cout << "\e[36m\e[1mFilterNode \e[0m \"" << m_instanceName << "\": execute, from "
                  << input->getData().size() << " to " << m_elements.size() << " "
                  << typeid( T ).name() << "." << std::endl;
#endif
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
                                     std::function<bool( v_t )> filterFunction ) :
    Node( instanceName, typeName ), m_filterFunction( filterFunction ) {
    auto portIn = new PortIn<coll_t>( "in", this );
    addInput( portIn );
    portIn->mustBeLinked();
    auto portOut = new PortOut<coll_t>( "out", this );
    addOutput( portOut, &m_elements );
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] =
        std::string { "Filtering function could not be serialized for " } + getTypeName();
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Filters
} // namespace Core
} // namespace Dataflow
} // namespace Ra
