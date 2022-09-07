#pragma once
#include <Dataflow/Core/DataflowGraph.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

inline void DataflowGraph::setNodeFactories( std::shared_ptr<NodeFactorySet> factories ) {
    m_factories = factories;
}
inline std::shared_ptr<NodeFactorySet> DataflowGraph::getNodeFactories() {
    return m_factories;
}

inline void DataflowGraph::addFactory( const std::string& name, std::shared_ptr<NodeFactory> f ) {
    if ( !m_factories ) { m_factories.reset( new NodeFactorySet ); }
    m_factories->addFactory( name, f );
}

inline const std::vector<std::shared_ptr<Node>>* DataflowGraph::getNodes() const {
    return &m_nodes;
}
inline const std::vector<std::vector<Node*>>* DataflowGraph::getNodesByLevel() const {
    return &m_nodesByLevel;
}
inline size_t DataflowGraph::getNodesCount() {
    return m_nodes.size();
}
inline const std::string DataflowGraph::getTypename() {
    return "DataFlow Graph";
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
