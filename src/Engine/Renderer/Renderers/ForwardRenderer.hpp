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

/// The ForwardRenderer class is the main renderer.
/// It implements forward rendering.
class RA_ENGINE_API ForwardRenderer : public Renderer {
  public:
    ForwardRenderer();
    ~ForwardRenderer();

    std::string getRendererName() const override { return "Forward Renderer"; }

  protected:
    void initializeInternal() override;
    void resizeInternal() override;

    void updateStepInternal( const RenderData& renderData ) override;

    void postProcessInternal( const RenderData& renderData ) override;
    void renderInternal( const RenderData& renderData ) override;
    void debugInternal( const RenderData& renderData ) override;
    void uiInternal( const RenderData& renderData ) override;

  private:
    /// Initialize the composition shaders.
    void initShaders();

    /// Initialize all the OpenGL buffers (Textures, FBOs).
    void initBuffers();

    /// Update the textures for shadows.
    /// \note Not implemented yet.
    void updateShadowMaps();

  protected:
    /// Used to indicate the texture to consider.
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

    // Default renderer logic here, no need to be accessed by overriding renderers.
    /// The list of FBOs used for the forward rendering.
    std::unique_ptr<globjects::Framebuffer> m_fbo;

    /// The list of FBOs used for post process.
    std::unique_ptr<globjects::Framebuffer> m_postprocessFbo;

    /// The list of FBOs used for rendering transparent RenderObjects.
    std::unique_ptr<globjects::Framebuffer> m_oitFbo;

    /// The list of transparent RenderObjects in the scene.
    std::vector<RenderObjectPtr> m_transparentRenderObjects;

    /// The number of transparent RenderObjects in the scene.
    uint m_fancyTransparentCount;

    /// The size for the ping-pong texture.
    /// \note A power of 2.
    uint m_pingPongSize;

    /// The list of textures used for rendering.
    std::array<std::unique_ptr<Texture>, RendererTexture_Count> m_textures;

    /// The size for the shadow textures.
    static const int ShadowMapSize = 1024;

    /// The list of shadow textures.
    std::vector<std::shared_ptr<Texture>> m_shadowMaps;

    /// The transformations for each Light in the scene.
    std::vector<Core::Matrix4> m_lightMatrices;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
