//
// Created by fseyedeh on 7/19/17.
//

#ifndef RADIUMENGINE_TEMPRENDERER_HPP
#define RADIUMENGINE_TEMPRENDERER_HPP


#include <globjects/Framebuffer.h>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra
{
    namespace Engine
    {
        class Texture;
    }
}

namespace Ra
{
    namespace Engine
    {
        class RA_ENGINE_API TempRenderer : public Renderer
        {
        public:
            TempRenderer( uint width, uint height );
            virtual ~TempRenderer();

            virtual std::string getRendererName() const override { return "Temp Renderer"; }

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

        private:
            enum RendererTextures
            {
                RendererTextures_Depth,
                RendererTextures_Normal,
                RendererTextures_Position,
                RendererTextures_Quad,
                RendererTexture_Count
            };

            // Default renderer logic here, no need to be accessed by overriding renderers.
            std::unique_ptr<globjects::Framebuffer> m_fbo;
            std::unique_ptr<globjects::Framebuffer> m_postprocessFbo;

            uint m_pingPongSize;

            std::vector<RenderObjectPtr> m_transparentRenderObjects;
            uint m_fancyTransparentCount;
            
            std::array<std::unique_ptr<Texture>, RendererTexture_Count> m_textures;

            bool m_showPos=false;
            int m_neighSize=0;
            double m_dThresh = 0;
            bool m_planeFit = false;
            double m_radius = 0;
            int m_depthCalc;
        public:
            void setShowPos(bool showPos);
            void setPlaneFit(bool planeFit);
            void setNeighSize(int neighSize);
            void setDepthThresh(double dThresh);
            void setRadius(double radius);
            void setDepthCalc(int index);
        };

    } // namespace Engine
} // namespace Ra

#endif //RADIUMENGINE_TEMPRENDERER_HPP
