#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <Engine/Renderer/Renderer.hpp>

namespace globjects {
class Framebuffer;
} // namespace globjects

namespace Ra {
namespace Engine {
class Texture;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
/**
 * Default renderer for the Radium Engine.
 * This classe implements aforward rendering algorithm with Z-prepass,
 * multipass light accumulation for opaque and transparent objects.
 * Once renderered, the result image is composited with gui, debug and X-ray-ed
 * objects renderings on demand.
 *
 * \see The Radium Engine default rendering information documentation
 *      for description of the Renderer.
 */
class RA_ENGINE_API ForwardRenderer : public Renderer {
  public:
    ForwardRenderer();

    ~ForwardRenderer() override;

    std::string getRendererName() const override { return "Forward Renderer"; }

  protected:
    void initializeInternal() override;

    void resizeInternal() override;

    void updateStepInternal( const ViewingParameters& renderData ) override;

    void postProcessInternal( const ViewingParameters& renderData ) override;

    void renderInternal( const ViewingParameters& renderData ) override;

    void debugInternal( const ViewingParameters& renderData ) override;

    void uiInternal( const ViewingParameters& renderData ) override;

  private:
    /**
     * Initialize the composition shaders.
     */
    void initShaders();

    /**
     * Initialize all the OpenGL buffers (Textures, FBOs).
     */
    void initBuffers();

    /**
     * Update the textures for shadows.
     * \note Not implemented yet.
     */
    void updateShadowMaps();

  protected:
    /**
     * Type of rendering output texture.
     */
    enum RendererTextures {
        RendererTextures_Depth = 0,
        RendererTextures_HDR,
        RendererTextures_Normal,
        RendererTextures_Diffuse,
        RendererTextures_Specular,
        RendererTextures_OITAccum,
        RendererTextures_OITRevealage,
        RendererTexture_Count
    };

  protected:
    // Default renderer logic here, no need to be accessed by overriding renderers.
    /// The list of FBOs used for the forward rendering.
    std::unique_ptr<globjects::Framebuffer> m_fbo;

    /// The list of FBOs used for post process.
    std::unique_ptr<globjects::Framebuffer> m_postprocessFbo;

    /// The list of FBOs used for rendering transparent RenderObjects.
    std::unique_ptr<globjects::Framebuffer> m_oitFbo;

    /// The list of FBOs used for rendering gui and x-ray-ed RenderObjects.
    std::unique_ptr<globjects::Framebuffer> m_uiXrayFbo;

    /// The list of transparent RenderObjects in the scene.
    std::vector<RenderObjectPtr> m_transparentRenderObjects;

    /// The number of transparent RenderObjects in the scene.
    size_t m_fancyTransparentCount{0};

    /**
     * The size for the ping-pong texture.
     * \note Must be a power of 2.
     */
    size_t m_pingPongSize{0};

    /// The list of Textures used for rendering.
    std::array<std::unique_ptr<Texture>, RendererTexture_Count> m_textures;

    /// The list of shadow Textures.
    std::vector<std::shared_ptr<Texture>> m_shadowMaps;

    /// The transformations for each Light in the scene.
    std::vector<Core::Matrix4> m_lightMatrices;

    /// The size for the shadow Textures.
    static const size_t ShadowMapSize{1024};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
