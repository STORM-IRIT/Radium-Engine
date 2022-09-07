#pragma once
#include <Dataflow/Core/Nodes/Filters/FilterNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Filters {

template <typename T>
FilterNode<T>::FilterNode( const std::string& instanceName ) :
    FilterNode( instanceName, getTypename(), []( T ) { return true; } ) {}

template <typename T>
FilterNode<T>::FilterNode( const std::string& instanceName,
                           FilterNode::UnaryPredicate filterFunction ) :
    FilterNode( instanceName, getTypename(), filterFunction ) {}

template <typename T>
void FilterNode<T>::setFilterFunction( UnaryPredicate filterFunction ) {
    m_filterFunction = filterFunction;
}

template <typename T>
void FilterNode<T>::init() {
    Node::init();
    m_elements.clear();
}

template <typename T>
void FilterNode<T>::execute() {
    auto input = dynamic_cast<PortIn<std::vector<T>>*>( m_inputs[0].get() );
    if ( input->isLinked() ) {
        const auto& inData = input->getData();
        m_elements.clear();
        m_elements.reserve( inData.size() );
        std::copy_if(
            inData.begin(), inData.end(), std::back_inserter( m_elements ), m_filterFunction );
#ifdef GRAPH_CALL_TRACE
        std::cout << "\e[36m\e[1mFilterNode \e[0m \"" << m_instanceName << "\": execute, from "
                  << input->getData().size() << " to " << m_elements.size() << " "
                  << typeid( T ).name() << "." << std::endl;
#endif
    }
}

template <typename T>
const std::string FilterNode<T>::getTypename() {
    return std::string { "Filter::" } + Ra::Core::Utils::demangleType<UnaryPredicate>();
}

template <typename T>
FilterNode<T>::FilterNode( const std::string& instanceName,
                           const std::string& typeName,
                           std::function<bool( T )> filterFunction ) :
    Node( instanceName, typeName ), m_filterFunction( filterFunction ) {
    auto portIn = new PortIn<std::vector<T>>( "in", this );
    addInput( portIn );
    portIn->mustBeLinked();
    auto portOut = new PortOut<std::vector<T>>( "out", this );
    addOutput( portOut, &m_elements );
}

template <typename T>
void FilterNode<T>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] =
        std::string { "Filtering function could not be serialized for " } + getTypeName();
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename T>
void FilterNode<T>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Filters
} // namespace Core
} // namespace Dataflow
} // namespace Ra
