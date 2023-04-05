#pragma once
#include <Dataflow/RaDataflow.hpp>

/**
 * Right now a renderer own a graph with some specific nodes available that gives acces to the
 * renderobjects, the camera and the lights.
 * These nodes are put in a specific factory SceneAccessors which contains nodes to access the
 * renderobject, the camera and the light of the scene.
 *
 */

#include <Core/Resources/Resources.hpp>
#include <Engine/Rendering/Renderer.hpp>

#include <Dataflow/Rendering/RenderingGraph.hpp>

namespace globjects {
class Framebuffer;
}

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Renderer {

/** Dataflow renderer for the Radium Engine
 * This Renderer is fully configurable, either dynamically or programmatically.
 * It implements the Ra::Engine::Rendering/Renderer interface.
 *
 * A NodeBasedRenderer is configured by using the RenderControlFunctor given at construction.
 * A RenderControlFunctor offers the following services :
 *  - configure() : add to the renderer as many RadiumNBR::RenderPass as needed. This method is
 *  called once when initializing the renderer. This method could also initialize internal
 *  resources into the controller that could be used to control the rendering.
 *  - resize() : called each time the renderer output is resized. This will allow modify controller
 *  resources that depends on the size of the output (e.g. internal textures ...)
 *  - update() : Called once before each frame to update the internal state of the renderer.
 *
 * A NodeBasedRenderer defines two textures that might be shared between passes :
 *  - a depth buffer attachable texture, stored with the key "Depth (RadiumNBR)" into the shared
 *  textures collection
 *  - a Linear space RGBA color texture, stored with the key "Linear RGB (RadiumNBR)" into the
 *  shared textures collection
 *
 *  If requested on the base Ra::Engine::Rendering::Renderer, a  NodeBasedRenderer apply a
 *  post-process step on the "Linear RGB (RadiumNBR)" that convert colors from linearRGB to sRGB
 *  color space  before displaying the image.
 *
 *
 * @see rendering.md for description of the renderer
 */
class RA_DATAFLOW_API DataflowRenderer : public Ra::Engine::Rendering::Renderer
{

  public:
    /**
     * Renderer controller
     */
    class RA_DATAFLOW_API RendererController : public Ra::Core::Resources::ObservableVoid
    {

      public:
        RendererController();
        virtual ~RendererController()                              = default;
        RendererController( const RendererController& )            = delete;
        RendererController( const RendererController&& )           = delete;
        RendererController& operator=( RendererController&& )      = delete;
        RendererController& operator=( const RendererController& ) = delete;

        /// Configuration function.
        /// Called once at the configuration of the renderer
        virtual void configure( ControllableRenderer* renderer, int w, int h );

        /// Resize function
        /// Called each time the renderer is resized
        virtual void resize( int w, int h );

        /// Update function
        /// Called once before each frame to update the internal state of the renderer
        virtual void update( const Ra::Engine::Data::ViewingParameters& renderData ) = 0;

        /// RenderTechnique builder
        /// Called each time the render techniques should be built (aftre switching from renderer,
        /// loading a scene, ...).
        virtual bool buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const = 0;

        /// Render the given scene
        /// \return true if rendering output is available
        virtual const std::vector<TextureType*>& render( std::vector<RenderObjectPtrType>* ros,
                                                         std::vector<LightPtrType>* lights,
                                                         const CameraType& cameras ) const = 0;

        [[nodiscard]] virtual std::string getRendererName() const {
            return "Base RendererController";
        }

      protected:
        ControllableRenderer* m_attachedRenderer;
        Ra::Engine::Data::ShaderProgramManager* m_shaderMngr;
        int m_width { -1 };
        int m_height { -1 };
    };

    /// Construct a renderer configured and managed through the controller
    explicit ControllableRenderer( RendererController& controller );

    /// The destructor is used to destroy the render graph
    ~ControllableRenderer() override;

    [[nodiscard]] std::string getRendererName() const override { return m_name; }

    bool buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const override;

    /// Access the default light manager
    Ra::Engine::Scene::LightManager* getLightManager() { return m_lightmanagers[0]; }

    /// Access the controller
    RendererController& getController() { return m_controller; }

  protected:
    void initializeInternal() override;
    void resizeInternal() override;
    void updateStepInternal( const Ra::Engine::Data::ViewingParameters& renderData ) override;
    void renderInternal( const Ra::Engine::Data::ViewingParameters& renderData ) override;
    void postProcessInternal( const Ra::Engine::Data::ViewingParameters& renderData ) override;
    void debugInternal( const Ra::Engine::Data::ViewingParameters& renderData ) override;
    void uiInternal( const Ra::Engine::Data::ViewingParameters& renderData ) override;

    /** Initialize internal resources for the renderer.
     * The base function creates the depth and final color texture to be shared by the rendering
     * passes that need them.
     */
    virtual void initResources();

  public:
    inline std::map<std::string, std::shared_ptr<Ra::Engine::Data::Texture>>& sharedTextures() {
        return m_sharedTextures;
    }

    inline globjects::Framebuffer* postprocessFbo() { return m_postprocessFbo.get(); }

    inline std::vector<RenderObjectPtr>* allRenderObjects() { return &m_fancyRenderObjects; }

    inline std::vector<const Ra::Engine::Scene::Light*>* getLights() { return &m_lights; }

  private:
    /// Controller observer method
    void controllerStateChanged();

    bool m_controllerStateChanged { false };

    /// textures own by the Renderer but shared across passes
    std::map<std::string, std::shared_ptr<Ra::Engine::Data::Texture>> m_sharedTextures;

    /// internal FBO used for post-processing
    std::unique_ptr<globjects::Framebuffer> m_postprocessFbo;

    /// The configurator functor to use
    RendererController& m_controller;

    /// The name of the renderer
    std::string m_name { "RenderGraph renderer" };

    /// Texture to be read for postprocess according to the display node
    Ra::Engine::Data::Texture* m_colorTexture { nullptr };

    /// Vector of lights ...
    std::vector<const Ra::Engine::Scene::Light*> m_lights;
};

} // namespace Renderer
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
