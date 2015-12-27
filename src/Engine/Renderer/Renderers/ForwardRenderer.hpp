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

            virtual void displayTexture( uint texIdx ) override;
            virtual std::vector<std::string> getAvailableTextures() const override;

            virtual std::string getRendererName() const override { return "Forward Renderer"; }

        protected:

            virtual void initializeInternal();
            virtual void resizeInternal();

            virtual void renderInternal( const RenderData& renderData ) override;
            virtual void postProcessInternal( const RenderData& renderData ) override;

        private:
            void initShaders();
            void initBuffers();

        private:
            enum RenderPassTextures
            {
                RENDERPASS_TEXTURE_DEPTH = 0,
                RENDERPASS_TEXTURE_AMBIENT,
                RENDERPASS_TEXTURE_POSITION,
                RENDERPASS_TEXTURE_NORMAL,
                RENDERPASS_TEXTURE_LIGHTED,
                RENDERPASS_TEXTURE_COUNT
            };

            enum OITPassTextures
            {
                OITPASS_TEXTURE_ACCUM,
                OITPASS_TEXTURE_REVEALAGE,
                OITPASS_TEXTURE_COUNT
            };

            // Default renderer logic here, no need to be accessed by overriding renderers.
            ShaderProgram* m_depthAmbientShader;
            ShaderProgram* m_renderpassCompositingShader;
            ShaderProgram* m_postprocessShader;
            ShaderProgram* m_drawScreenShader;

            std::unique_ptr<Mesh> m_quadMesh;

            std::unique_ptr<FBO> m_fbo;
            std::unique_ptr<FBO> m_postprocessFbo;

            std::unique_ptr<Texture> m_renderpassTexture;
            std::array<std::unique_ptr<Texture>, RENDERPASS_TEXTURE_COUNT> m_renderpassTextures;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
