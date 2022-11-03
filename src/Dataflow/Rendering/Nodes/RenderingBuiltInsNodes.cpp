#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Rendering/Nodes/Sources/Scene.hpp>

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

    /* -- end --*/
    Core::NodeFactoriesManager::registerFactory( renderingFactory );
}

} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
