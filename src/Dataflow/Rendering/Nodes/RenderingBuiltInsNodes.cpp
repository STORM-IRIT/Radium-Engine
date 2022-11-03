#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Rendering/Nodes/Sinks/DisplaySinkNode.hpp>
#include <Dataflow/Rendering/Nodes/Sources/Scene.hpp>
#include <Dataflow/Rendering/Nodes/Sources/TextureSourceNode.hpp>

#include <Dataflow/Rendering/Nodes/RenderNodes/ClearColorNode.hpp>

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
    renderingFactory->registerNodeCreator<Nodes::ColorTextureNode>(
        Nodes::ColorTextureNode::getTypename() + "_", "Source" );
    renderingFactory->registerNodeCreator<Nodes::DepthTextureNode>(
        Nodes::DepthTextureNode::getTypename() + "_", "Source" );
    /* --- Sinks --- */
    renderingFactory->registerNodeCreator<Nodes::DisplaySinkNode>(
        Nodes::DisplaySinkNode::getTypename() + "_", "Sinks" );

    /* --- operators --- */
    renderingFactory->registerNodeCreator<Nodes::ClearColorNode>(
        Nodes::ClearColorNode::getTypename() + "_", "Render" );

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
