#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

#if 0
#    include <RadiumRenderNodes/Nodes/Scene/SceneNode.hpp>
#    include <RadiumRenderNodes/Nodes/Sinks/DisplaySinkNode.hpp>
#endif

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Renderer {
using namespace Ra::Dataflow::Rendering::Nodes;
using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API RenderingGraph : public DataflowGraph
{
  public:
    RenderingGraph( const std::string& name );
    ~RenderingGraph() override = default;

    void init() override;
    bool addNode( Node* newNode ) override;
    bool removeNode( Node* node ) override;
    void clearNodes() override;

    /// Sets the shader program manager
    void setShaderProgramManager( Ra::Engine::Data::ShaderProgramManager* shaderMngr ) {
        m_shaderMngr = shaderMngr;
    }

    /// Resize all the rendering output
    void resize( uint32_t width, uint32_t height );

    /// Set the scene accessors on the graph
    void setDataSources( std::vector<RenderObjectType>* ros, std::vector<LightType>* lights );
    /// Set the viewpoint on the graph
    void setCameras( std::vector<CameraType>* cameras );

    /// get the computed texture vector
    const std::vector<TextureType*>& getImagesOutput() const;

    /// Set render techniques needed by the rendering nodes
    void buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const;
    /// Return the typename of the Graph
    static const std::string& getTypename();

  protected:
    bool postCompilationOperation() override;
    void fromJsonInternal( const nlohmann::json& ) override;
    void toJsonInternal( nlohmann::json& ) const override;

  private:
    /// The renderer's shader program manager
    Ra::Engine::Data::ShaderProgramManager* m_shaderMngr;
    /// List of nodes that requires some particular processing
    std::vector<RenderingNode*> m_renderingNodes; // to resize
    std::vector<RenderingNode*> m_rtIndexedNodes; // associate an index and buildRenderTechnique
#if 0
    /// List of nodes that serve as data provider
    std::vector<SceneNode*> m_dataProviders;

    // DisplaySink observerMethod : right now, observe only displaySink node
    void observeSinks( const std::vector<TextureType*>& graphOutput );
    /// The display sink node used to get rendered images
    DisplaySinkNode* m_displaySinkNode { nullptr };
#endif
    /// ObserverId for displaySink;
    int m_displayObserverId { -1 };
    std::vector<TextureType*> m_outputTextures;
};

} // namespace Renderer
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Rendering/Renderer/RenderingGraph.inl>
