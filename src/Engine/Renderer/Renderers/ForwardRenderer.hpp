#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <Engine/Renderer/Renderer.hpp>

namespace globjects {
class Framebuffer;
}

namespace Ra {
namespace Engine {
class Texture;
}
} // namespace Ra

namespace Ra {
namespace Engine {
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
    void initShaders();
    void initBuffers();

    void updateShadowMaps();

  protected:
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
    std::unique_ptr<globjects::Framebuffer> m_fbo;
    std::unique_ptr<globjects::Framebuffer> m_postprocessFbo;
    std::unique_ptr<globjects::Framebuffer> m_oitFbo;

    std::vector<RenderObjectPtr> m_transparentRenderObjects;
    uint m_fancyTransparentCount;

    uint m_pingPongSize;

    std::array<std::unique_ptr<Texture>, RendererTexture_Count> m_textures;

    static const int ShadowMapSize = 1024;
    std::vector<std::shared_ptr<Texture>> m_shadowMaps;
    std::vector<Core::Matrix4> m_lightMatrices;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
