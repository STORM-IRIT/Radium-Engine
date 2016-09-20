#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra
{
    namespace Engine
    {
        class RA_ENGINE_API ForwardRenderer : public Renderer
        {
        public:
            ForwardRenderer( uint width, uint height );
            virtual ~ForwardRenderer();

            virtual std::string getRendererName() const override { return "Forward Renderer"; }

        protected:

            virtual void initializeInternal() override;
            virtual void resizeInternal() override;

            virtual void updateStepInternal( const RenderData& renderData ) override;

            virtual void postProcessInternal( const RenderData& renderData ) override;
            virtual void renderInternal( const RenderData& renderData ) override;
            virtual void debugInternal( const RenderData& renderData ) override;
            virtual void uiInternal( const RenderData& renderData ) override;

        private:
            void initShaders();
            void initBuffers();

            void updateShadowMaps();

        private:
            enum RendererTextures
            {
                TEX_DEPTH = 0,
                TEX_NORMAL,
                TEX_LIT,
                TEX_LUMINANCE,
                TEX_TONEMAPPED,
                TEX_TONEMAP_PING,
                TEX_TONEMAP_PONG,
                TEX_BLOOM_PING,
                TEX_BLOOM_PONG,
                TEX_OIT_TEXTURE_ACCUM,
                TEX_OIT_TEXTURE_REVEALAGE,
                TEX_COUNT
            };

            // Default renderer logic here, no need to be accessed by overriding renderers.
            std::unique_ptr<FBO> m_fbo;
            std::unique_ptr<FBO> m_postprocessFbo;
            std::unique_ptr<FBO> m_pingPongFbo;
            std::unique_ptr<FBO> m_bloomFbo;

            std::unique_ptr<FBO> m_shadowFbo;

            std::unique_ptr<FBO> m_oitFbo;
            
            std::vector<RenderObjectPtr> m_transparentRenderObjects;
            uint m_fancyTransparentCount;

            uint m_pingPongSize;

            std::array<std::unique_ptr<Texture>, TEX_COUNT> m_textures;

            static const int ShadowMapSize = 1024;
            std::vector<std::shared_ptr<Texture>> m_shadowMaps;
            std::vector<Core::Matrix4> m_lightMatrices;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
