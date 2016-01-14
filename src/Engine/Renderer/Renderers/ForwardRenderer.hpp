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
            virtual void renderInternal( const RenderData& renderData ) override;
            virtual void postProcessInternal( const RenderData& renderData ) override;

        private:
            void initShaders();
            void initBuffers();

        private:
            enum RenderPassTextures
            {
                TEX_DEPTH = 0,
                TEX_NORMAL,
                TEX_LIGHTED,
                TEX_COUNT
            };

            // Default renderer logic here, no need to be accessed by overriding renderers.
            ShaderProgram* m_depthAmbientShader;
            ShaderProgram* m_postprocessShader;

            std::unique_ptr<Mesh> m_quadMesh;

            std::unique_ptr<FBO> m_fbo;
            std::unique_ptr<FBO> m_postprocessFbo;

            std::array<std::unique_ptr<Texture>, TEX_COUNT> m_textures;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
