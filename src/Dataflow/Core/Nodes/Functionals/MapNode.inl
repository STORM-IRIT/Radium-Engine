#pragma once
#include <Dataflow/Core/Nodes/Functionals/MapNode.hpp>

#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

template <typename coll_t, typename v_t>
MapNode<coll_t, v_t>::MapNode( const std::string& instanceName ) :
    MapNode( instanceName, getTypename(), []( v_t ) { return v_t {}; } ) {}

template <typename coll_t, typename v_t>
MapNode<coll_t, v_t>::MapNode( const std::string& instanceName, MapOperator mapOperator ) :
    MapNode( instanceName, getTypename(), mapOperator ) {}

template <typename coll_t, typename v_t>
void MapNode<coll_t, v_t>::setFilterFunction( MapOperator mapOperator ) {
    m_mapOperator = mapOperator;
}

template <typename coll_t, typename v_t>
void MapNode<coll_t, v_t>::init() {
    Node::init();
    m_elements.clear();
}

template <typename coll_t, typename v_t>
void MapNode<coll_t, v_t>::execute() {
    auto predPort = dynamic_cast<PortIn<MapOperator>*>( m_inputs[1].get() );
    auto f        = m_mapOperator;
    if ( predPort->isLinked() ) { f = predPort->getData(); }
    auto input = dynamic_cast<PortIn<coll_t>*>( m_inputs[0].get() );
    if ( input->isLinked() ) {
        const auto& inData = input->getData();
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
const std::string& MapNode<coll_t, v_t>::getTypename() {
    static std::string demangledName =
        std::string { "Map<" } + Ra::Dataflow::Core::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
MapNode<coll_t, v_t>::MapNode( const std::string& instanceName,
                               const std::string& typeName,
                               MapOperator mapOperator ) :
    Node( instanceName, typeName ), m_mapOperator( mapOperator ) {
    auto portIn = new PortIn<coll_t>( "in", this );
    addInput( portIn );
    portIn->mustBeLinked();

    auto op = new PortIn<MapOperator>( "f", this );
    addInput( op );

    auto portOut = new PortOut<coll_t>( "out", this );
    addOutput( portOut, &m_elements );
}

template <typename coll_t, typename v_t>
void MapNode<coll_t, v_t>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] = std::string { "Map operator could not be serialized for " } + getTypeName();
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename coll_t, typename v_t>
void MapNode<coll_t, v_t>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
