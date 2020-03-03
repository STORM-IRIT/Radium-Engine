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
/** Default renderer for the Radium Engine
 * This classe implements aforward rendering algorithm with Z-prepass, multipass light accumulation
 * for opaque and transperent objects. Once renderer, the final is composited with Ui, debug and
 * X-ray objects renderings on demand.
 *
 * @see rendering.md for description of the renderer
 */
class RA_ENGINE_API ForwardRenderer : public Renderer
{
  public:
    ForwardRenderer();
    ~ForwardRenderer() override;

    std::string getRendererName() const override { return "Forward Renderer"; }
    bool buildRenderTechnique( RenderObject* ro ) const override;

  protected:
    void initializeInternal() override;
    void resizeInternal() override;

    void updateStepInternal( const ViewingParameters& renderData ) override;

    void postProcessInternal( const ViewingParameters& renderData ) override;
    void renderInternal( const ViewingParameters& renderData ) override;
    void debugInternal( const ViewingParameters& renderData ) override;
    void uiInternal( const ViewingParameters& renderData ) override;

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
        RendererTextures_Volume,
        RendererTexture_Count
    };

    // Default renderer logic here, no need to be accessed by overriding renderers.
    std::unique_ptr<globjects::Framebuffer> m_fbo;
    std::unique_ptr<globjects::Framebuffer> m_postprocessFbo;
    std::unique_ptr<globjects::Framebuffer> m_oitFbo;
    std::unique_ptr<globjects::Framebuffer> m_uiXrayFbo;
    std::unique_ptr<globjects::Framebuffer> m_volumeFbo;

    std::vector<RenderObjectPtr> m_transparentRenderObjects;
    size_t m_fancyTransparentCount{0};

    std::vector<RenderObjectPtr> m_volumetricRenderObjects;
    size_t m_fancyVolumetricCount{0};

    size_t m_pingPongSize{0};

    std::array<std::unique_ptr<Texture>, RendererTexture_Count> m_textures;

    static const size_t ShadowMapSize{1024};
    std::vector<std::shared_ptr<Texture>> m_shadowMaps;
    std::vector<Core::Matrix4> m_lightMatrices;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
