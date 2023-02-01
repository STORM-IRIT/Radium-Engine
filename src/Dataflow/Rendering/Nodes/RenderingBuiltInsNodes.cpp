#include <Dataflow/Core/DataflowGraph.hpp>
DATAFLOW_LIBRARY_INITIALIZER_DECL( RenderingNodes );

#include <Dataflow/Rendering/Nodes/Sinks/DisplaySinkNode.hpp>
#include <Dataflow/Rendering/Nodes/Sources/EnvMapSourceNode.hpp>
#include <Dataflow/Rendering/Nodes/Sources/Scene.hpp>
#include <Dataflow/Rendering/Nodes/Sources/TextureSourceNode.hpp>

#include <Dataflow/Rendering/Nodes/RenderNodes/ClearColorNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/EmissivityRenderNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/EnvironmentLightingNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/GeometryAovsNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/LocalLightingNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/SimpleRenderNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/SsaoRenderNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/TransparentLocalLightingNode.hpp>

#include <Dataflow/Rendering/RenderingGraph.hpp>

#include <Core/Resources/Resources.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {

namespace Nodes {
// This is here right now, can be moved if requested :
std::string RenderingNode::s_defaultResourceDir;
} // namespace Nodes

/// Todo, put this somewhere else. This is needed to locate resources by client applications
/// Todo (bis), remove this requirement
static int DataflowRendererMagic = 0x01020304;

std::string registerRenderingNodesFactories() {
    auto resourcesCheck = Ra::Core::Resources::getResourcesPath(
        reinterpret_cast<void*>( &DataflowRendererMagic ), { "Resources/Radium/Dataflow" } );
    if ( !resourcesCheck ) {
        LOG( Ra::Core::Utils::logERROR ) << "Unable to find resources for renderingNodeFactory, "
                                            "setting it to current directory!";
        resourcesCheck = "./";
    }
    auto resourcesPath { *resourcesCheck };

    Core::NodeFactorySet::mapped_type renderingFactory {
        new Core::NodeFactorySet::mapped_type::element_type( "RenderingNodes" ) };

    /* --- Sources --- */
    renderingFactory->registerNodeCreator<Nodes::SceneNode>( Nodes::SceneNode::getTypename() + "_",
                                                             "Source" );
    renderingFactory->registerNodeCreator<Nodes::ColorTextureNode>(
        Nodes::ColorTextureNode::getTypename() + "_", "Source" );
    renderingFactory->registerNodeCreator<Nodes::DepthTextureNode>(
        Nodes::DepthTextureNode::getTypename() + "_", "Source" );

    renderingFactory->registerNodeCreator<Nodes::EnvMapSourceNode>(
        Nodes::EnvMapSourceNode::getTypename() + "_", "Source" );

    /* --- Sinks --- */
    renderingFactory->registerNodeCreator<Nodes::DisplaySinkNode>(
        Nodes::DisplaySinkNode::getTypename() + "_", "Sinks" );

    /* --- operators --- */
    renderingFactory->registerNodeCreator<Nodes::ClearColorNode>(
        Nodes::ClearColorNode::getTypename() + "_", "Render" );

    renderingFactory->registerNodeCreator<SimpleRenderNode>(
        [resourcesPath, renderingFactory]( const nlohmann::json& data ) {
            auto node = new SimpleRenderNode( "SimpleRender_" +
                                              std::to_string( renderingFactory->nextNodeId() ) );
            node->fromJson( data );
            node->setResourcesDir( resourcesPath );
            return node;
        },
        "Render" );

    renderingFactory->registerNodeCreator<GeometryAovsNode>(
        [resourcesPath, renderingFactory]( const nlohmann::json& data ) {
            auto node = new GeometryAovsNode( "GeometryAovs_" +
                                              std::to_string( renderingFactory->nextNodeId() ) );
            node->fromJson( data );
            node->setResourcesDir( resourcesPath );
            return node;
        },
        "Render" );

    renderingFactory->registerNodeCreator<EmissivityNode>(
        [resourcesPath, renderingFactory]( const nlohmann::json& data ) {
            auto node = new EmissivityNode( "Emissivity_" +
                                            std::to_string( renderingFactory->nextNodeId() ) );
            node->fromJson( data );
            node->setResourcesDir( resourcesPath );
            return node;
        },
        "Render" );

    renderingFactory->registerNodeCreator<EnvironmentLightingNode>(
        [resourcesPath, renderingFactory]( const nlohmann::json& data ) {
            auto node = new EnvironmentLightingNode(
                "EnLighting_" + std::to_string( renderingFactory->nextNodeId() ) );
            node->fromJson( data );
            node->setResourcesDir( resourcesPath );
            return node;
        },
        "Render" );

    renderingFactory->registerNodeCreator<LocalLightingNode>(
        [resourcesPath, renderingFactory]( const nlohmann::json& data ) {
            auto node = new LocalLightingNode( "LocalLighting_" +
                                               std::to_string( renderingFactory->nextNodeId() ) );
            node->fromJson( data );
            node->setResourcesDir( resourcesPath );
            return node;
        },
        "Render" );

    renderingFactory->registerNodeCreator<TransparentLocalLightingNode>(
        [resourcesPath, renderingFactory]( const nlohmann::json& data ) {
            auto node = new TransparentLocalLightingNode(
                "TransparentLocalLighting_" + std::to_string( renderingFactory->nextNodeId() ) );
            node->fromJson( data );
            node->setResourcesDir( resourcesPath );
            return node;
        },
        "Render" );

    renderingFactory->registerNodeCreator<SsaoNode>(
        [resourcesPath, renderingFactory]( const nlohmann::json& data ) {
            auto node = new SsaoNode( "Ssao_" + std::to_string( renderingFactory->nextNodeId() ) );
            node->fromJson( data );
            node->setResourcesDir( resourcesPath );
            return node;
        },
        "Render" );

    /* --- Graphs --- */
    renderingFactory->registerNodeCreator<RenderingGraph>( RenderingGraph::getTypename() + "_",
                                                           "Graph" );

    /* -- end --*/
    Core::NodeFactoriesManager::registerFactory( renderingFactory );

    return resourcesPath;
}

} // namespace Rendering
} // namespace Dataflow
} // namespace Ra

DATAFLOW_LIBRARY_INITIALIZER_IMPL( RenderingNodes ) {
    Ra::Dataflow::Rendering::Nodes::RenderingNode::s_defaultResourceDir =
        Ra::Dataflow::Rendering::registerRenderingNodesFactories();
}
