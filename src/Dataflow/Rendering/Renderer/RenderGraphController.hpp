#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Rendering/Renderer/ControllableRenderer.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Renderer {

/**
 * RenderGraph controller
 */
class RA_DATAFLOW_API RenderGraphController : public ControllableRenderer::RendererController
{

  public:
    RenderGraphController();
    virtual ~RenderGraphController()                       = default;
    RenderGraphController( const RenderGraphController& )  = delete;
    RenderGraphController( const RenderGraphController&& ) = delete;
    RenderGraphController& operator=( RenderGraphController&& ) = delete;
    RenderGraphController& operator=( const RenderGraphController& ) = delete;

    /// Configuration function.
    /// Called once at the configuration of the renderer
    void configure( ControllableRenderer* renderer, int w, int h ) override;

    /// Resize function
    /// Called each time the renderer is resized
    void resize( int w, int h ) override;

    /// Update function
    /// Called once before each frame to update the internal state of the renderer
    void update( const Ra::Engine::Data::ViewingParameters& renderData ) override;

    /// RenderTechnique builder
    /// Called each time the render techniques should be built.
    bool buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const override;

    const std::vector<TextureType*>& render( std::vector<RenderObjectPtrType>* ros,
                                             std::vector<LightPtrType>* lights,
                                             const CameraType& cameras ) const override;

    [[nodiscard]] std::string getRendererName() const override { return "Node Renderer"; }

    void loadGraph( const std::string& filename );
    void saveGraph( const std::string& filename );
    void resetGraph();
    /// Call this to set a graph to load before OpenGL is OK
    void deferredLoadGraph( const std::string& filename );

    // allow to "edit" the graph
    RenderingGraph* getGraph() { return m_renderGraph.get(); }

  protected:
    /// The controlled graph.
    /// The controller own the graph and manage loading/saving of the renderer
    std::unique_ptr<RenderingGraph> m_renderGraph { nullptr };

    mutable std::vector<RenderingGraph::DataSetterDesc> m_renderGraphInputs;
    mutable std::vector<RenderingGraph::DataGetterDesc> m_renderGraphOutputs;
    mutable std::vector<TextureType*> m_images;

    std::string m_graphToLoad;

    void compile( bool notifyObservers = false ) const;
};

} // namespace Renderer
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
