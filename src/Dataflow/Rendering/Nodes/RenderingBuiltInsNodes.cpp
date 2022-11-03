#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Rendering/Nodes/Sources/Scene.hpp>
#include <Dataflow/Rendering/RenderingGraph.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {

void registerRenderingNodesFactories() {
    Core::NodeFactorySet::mapped_type renderingFactory {
        new Core::NodeFactorySet::mapped_type::element_type( "RenderingNodes" ) };

    /* --- Sources --- */
    renderingFactory->registerNodeCreator<Nodes::SceneNode>( Nodes::SceneNode::getTypename() + "_",
                                                             "Source" );

    /* --- Sinks --- */

    /* --- operators --- */

    /* --- Graphs --- */
    renderingFactory->registerNodeCreator<RenderingGraph>( RenderingGraph::getTypename() + "_",
                                                           "Graph" );

    /* -- end --*/
    Core::NodeFactoriesManager::registerFactory( renderingFactory );
}

} // namespace Rendering
} // namespace Dataflow
} // namespace Ra

DATAFLOW_LIBRARY_INITIALIZER( RenderingNodes ) {
    Ra::Dataflow::Rendering::registerRenderingNodesFactories();
}
