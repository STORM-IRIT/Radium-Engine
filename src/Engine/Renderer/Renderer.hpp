#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <Engine/RaEngine.hpp>

#include <vector>
#include <array>
#include <mutex>
#include <memory>
#include <chrono>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Time/Timer.hpp>
#include <Core/Event/EventEnums.hpp>

#include <Engine/Renderer/RenderQueue/RenderQueue.hpp>

namespace Ra
{
    namespace Core
    {
        struct MouseEvent;
        struct KeyEvent;
    }
}

namespace Ra
{
    namespace Engine
    {
        class Camera;
        class RenderObject;
        class FBO;
        class Light;
        class Mesh;
        class ShaderProgram;
        class ShaderProgramManager;
        class Texture;
        class TextureManager;
    }
}

namespace Ra
{
    namespace Engine
    {

        struct RA_ENGINE_API RenderData
        {
            Core::Matrix4 viewMatrix;
            Core::Matrix4 projMatrix;
            Scalar dt;
        };

        class RA_ENGINE_API Renderer
        {
        protected:
            typedef std::shared_ptr<RenderObject> RenderObjectPtr;

        public:
            enum TexturesFBO
            {
                TEXTURE_DEPTH = 0,
                TEXTURE_AMBIENT,
                TEXTURE_POSITION,
                TEXTURE_NORMAL,
                TEXTURE_COLOR,
                TEXTURE_COUNT
            };

            struct TimerData
            {
                Core::Timer::TimePoint renderStart;
                Core::Timer::TimePoint updateEnd;
                Core::Timer::TimePoint feedRenderQueuesEnd;
                Core::Timer::TimePoint mainRenderEnd;
                Core::Timer::TimePoint postProcessEnd;
                Core::Timer::TimePoint renderEnd;
            };

            struct PickingQuery
            {
                Core::Vector2 m_screenCoords;
                Core::MouseButton::MouseButton m_button;
            };

        public:
            /// CONSTRUCTOR
            Renderer( uint width, uint height );

            /// DESCTRUCTOR
            virtual ~Renderer();

            virtual void initialize();

            const TimerData& getTimerData() const
            {
                return m_timerData;
            }

            // Lock the renderer (for MT access)
            void lockRendering()
            {
                m_renderMutex.lock();
            }
            void unlockRendering()
            {
                m_renderMutex.unlock();
            }

            /**
             * @brief Tell the renderer it needs to render.
             * This method does the following steps :
             * <ol>
             *   <li>call @see updateRenderObjectsInternal method</li>
             *   <li>call @see renderInternal method</li>
             *   <li>call @see postProcessInternal method</li>
             *   <li>render the final texture in the right framebuffer*</li>
             * </ol>
             *
             * @param renderData The basic data needed for the rendering :
             * Time elapsed since last frame, camera view matrix, camera projection matrix.
             *
             * @note * What "render in the right buffer" means, is that, for example,
             * when using QOpenGLWidget, Qt binds its own framebuffer before calling
             * updateGL() method.
             * So, render() takes that into account by saving an eventual bound
             * framebuffer, and restores it before drawing the last final texture.
             * If no framebuffer was bound, it draws into GL_BACK.
             */
            void render( const RenderData& renderData );

            /**
             * @brief Resize the viewport and all the screen textures, fbos.
             * This function must be overrided as soon as some FBO or screensized
             * texture is used (since the default implementation just resizes its
             * own fbos / textures)
             *
             * @param width The new viewport width
             * @param height The new viewport height
             */
            virtual void resize( uint width, uint height );

            /**
             * @brief Change the texture that is displayed on screen.
             * This must be overrided if you want to properly be able to
             * see your textures.
             *
             * @param texIdx The texture to display.
             */
            // FIXME(Charly): For now the drawn texture takes the whole viewport,
            //                maybe it could be great if we had a way to switch between
            //                the current "fullscreen" debug mode, and some kind of
            //                "windowed" mode (that would show the debugged texture in
            //                its own viewport, without hiding the final texture.)
            virtual void debugTexture( uint texIdx );


            // FIXME(Charly): Not sure the lights should be handled by the renderer.
            //                How to do this ?
            void addLight( const std::shared_ptr<Light>& light )
            {
                m_lights.push_back( light );
            }

            void reloadShaders();

            // FIXME(Charly): Maybe there is a better way to handle lights ?
            virtual void handleFileLoading( const std::string& filename );

            void addPickingRequest(const PickingQuery& query)
            {
                m_pickingQueries.push_back( query );
            }

            inline const std::vector<int>& getPickingResults() const
            {
                return m_pickingResults;
            }

            inline const std::vector<PickingQuery>& getPickingQueries() const
            {
                return m_lastFramePickingQueries;
            }

            inline void toggleDrawDebug()
            {
                m_drawDebug = !m_drawDebug;
            }

        protected:

            // 4.
            /**
              * @brief All the scene rendering magics basically happens here.
              *
              * @param renderData The basic data needed for the rendering :
              * Time elapsed since last frame, camera view matrix, camera projection matrix.
              */
            virtual void renderInternal( const RenderData& renderData );

            // 5.
            /**
             * @brief Do all post processing stuff. If you override this method,
             * be careful to fill @see m_finalTexture since it is the texture that
             * will be displayed at the very end of the @see render method.
             *
             * @param renderData The basic data needed for the rendering :
             * Time elapsed since last frame, camera view matrix, camera projection matrix.
             */
            virtual void postProcessInternal( const RenderData& renderData );

        private:

            // 0.
            void saveExternalFBOInternal();

            // 1.
            virtual void updateRenderObjectsInternal( const RenderData& renderData,
                                                      const std::vector<RenderObjectPtr>& renderObjects );

            // 2.
            virtual void feedRenderQueuesInternal( const RenderData& renderData,
                                                   const std::vector<RenderObjectPtr>& renderObjects );

            // 3.
            void doPicking();

            // 6.
            void drawScreenInternal();

            void initShaders();
            void initBuffers();

        protected:
            uint m_width;
            uint m_height;

            ShaderProgramManager* m_shaderManager;
            TextureManager* m_textureManager;

            // FIXME(Charly): Should we change "displayedTexture" to "debuggedTexture" ?
            //                It would make more sense if we are able to show the
            //                debugged texture in its own viewport.
            /**
             * @brief The texture that will be displayed on screen. If no call to
             * @see debugTexture has been done, this is just a pointer to
             * @see m_finalTexture.
             */
            Texture* m_displayedTexture;

            /**
             * @brief The texture that must be filled by the @see renderInternal method.
             */
            std::unique_ptr<Texture> m_renderpassTexture;

            /**
             * @brief The texture that must be filled by the @see postProcessInternal
             * method.
             */
            std::unique_ptr<Texture> m_finalTexture;

            /**
             * @brief Tell the DrawScreen shader if a depth map is beeing debugged.
             * If true, some depth linearization will be done for better vizualisation.
             */
            bool m_displayedIsDepth;

            std::vector<std::shared_ptr<Light>> m_lights;

            RenderQueue m_opaqueRenderQueue;
            RenderQueue m_transparentRenderQueue;
            RenderQueue m_xrayRenderQueue;
            RenderQueue m_debugRenderQueue;
            RenderQueue m_uiRenderQueue;

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
            ShaderProgram* m_oiTransparencyShader;
            ShaderProgram* m_postprocessShader;
            ShaderProgram* m_drawScreenShader;

            std::unique_ptr<Mesh> m_quadMesh;

            int m_qtPlz;

            std::unique_ptr<FBO> m_fbo;
            std::unique_ptr<FBO> m_oitFbo;
            std::unique_ptr<FBO> m_postprocessFbo;

            std::array<std::unique_ptr<Texture>, RENDERPASS_TEXTURE_COUNT> m_renderpassTextures;
            std::array<std::unique_ptr<Texture>, OITPASS_TEXTURE_COUNT> m_oitTextures;

            TimerData m_timerData;

            std::mutex m_renderMutex;

            // PICKING STUFF
            std::unique_ptr<FBO>        m_pickingFbo;
            std::unique_ptr<Texture>    m_pickingTexture;
            ShaderProgram*              m_pickingShader;

            std::vector<PickingQuery>   m_pickingQueries;
            std::vector<PickingQuery>   m_lastFramePickingQueries;
            std::vector<int>            m_pickingResults;

            bool m_drawDebug;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
