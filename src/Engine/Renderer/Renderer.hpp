#ifndef RADIUMENGINE_RENDERER_HPP
#define RADIUMENGINE_RENDERER_HPP

#include <Engine/RaEngine.hpp>

#include <vector>
#include <array>
#include <mutex>
#include <memory>
#include <chrono>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Time/Timer.hpp>
#include <Core/Event/EventEnums.hpp>


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
        class RenderObjectManager;
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
            Renderer( uint width, uint height );
            virtual ~Renderer();

            // -=-=-=-=-=-=-=-=- FINAL -=-=-=-=-=-=-=-=- //
            virtual const TimerData& getTimerData() const final
            {
                return m_timerData;
            }

            // Lock the renderer (for MT access)
            virtual void lockRendering() final
            {
                m_renderMutex.lock();
            }

            virtual void unlockRendering() final
            {
                m_renderMutex.unlock();
            }

            virtual void toggleWireframe() final
            {
                m_wireframe = !m_wireframe;
            }

            virtual void setWireframe(bool wireframe) final
            {
                m_wireframe = wireframe;
            }

            virtual void enablePostProcess(bool enabled) final
            {
                m_postProcessEnabled = enabled;
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
            virtual void render( const RenderData& renderData ) final;

            // -=-=-=-=-=-=-=-=- VIRTUAL -=-=-=-=-=-=-=-=- //
            /**
             * @brief Initialize renderer
             */
            virtual void initialize() final;

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

            // FIXME(Charly): Not sure the lights should be handled by the renderer.
            //                How to do this ?
            virtual void addLight( const std::shared_ptr<Light>& light )
            {
                m_lights.push_back( light );
            }

            virtual void reloadShaders();

            // FIXME(Charly): Maybe there is a better way to handle lights ?
            // FIXME(Charly): Final ?
            virtual void handleFileLoading( const std::string& filename ) final;

            virtual void addPickingRequest(const PickingQuery& query)
            {
                m_pickingQueries.push_back( query );
            }

            inline virtual const std::vector<int>& getPickingResults() const final
            {
                return m_pickingResults;
            }

            inline virtual const std::vector<PickingQuery>& getPickingQueries() const final
            {
                return m_lastFramePickingQueries;
            }

            inline virtual void toggleDrawDebug()
            {
                m_drawDebug = !m_drawDebug;
            }

            /**
             * @brief Change the texture that is displayed on screen.
             * Set m_displayedIsDepth to true if depth linearization is wanted
             *
             * @param texIdx The texture to display.
             */
            // FIXME(Charly): For now the drawn texture takes the whole viewport,
            //                maybe it could be great if we had a way to switch between
            //                the current "fullscreen" debug mode, and some kind of
            //                "windowed" mode (that would show the debugged texture in
            //                its own viewport, without hiding the final texture.)
            virtual void displayTexture( const std::string& texName ) final;

            /**
             * @brief Return the names of renderer available textures
             * @return A vector of strings, containing the name of the different textures
             */
            virtual std::vector<std::string> getAvailableTextures() const final;

            /**
             * @brief Get the name of the renderer, e.g to be displayed in the UI
             * @return
             */
            virtual std::string getRendererName() const = 0;

        protected:

            /**
             * @brief initializeInternal
             */
            virtual void initializeInternal() = 0;
            virtual void resizeInternal() = 0;

            // 2.1
            virtual void updateStepInternal( const RenderData& renderData ) = 0;

            // 4.
            /**
             * @brief All the scene rendering magics basically happens here.
             *
             * @param renderData The basic data needed for the rendering :
             * Time elapsed since last frame, camera view matrix, camera projection matrix.
             */
            // FIXME(Charly): pure virtual ?
            virtual void renderInternal( const RenderData& renderData ) = 0;

            // 5.
            /**
             * @brief Do all post processing stuff. If you override this method,
             * be careful to fill @see m_finalTexture since it is the texture that
             * will be displayed at the very end of the @see render method.
             *
             * @param renderData The basic data needed for the rendering :
             * Time elapsed since last frame, camera view matrix, camera projection matrix.
             */
            // FIXME(Charly): pure virtual ?
            virtual void postProcessInternal( const RenderData& renderData ) = 0;

        private:

            // 0.
            virtual void saveExternalFBOInternal() final;

            // 1.
            virtual void feedRenderQueuesInternal() final;

            // 2.0
            virtual void updateRenderObjectsInternal( const RenderData& renderData) final;

            // 3.
            virtual void doPicking( const RenderData& renderData ) final;

            // 6.
            virtual void drawScreenInternal() final;

            // 7.
            virtual void notifyRenderObjectsRenderingInternal() final;

        protected:
            uint m_width;
            uint m_height;

            ShaderProgramManager* m_shaderMgr;
            RenderObjectManager* m_roMgr;

            // FIXME(Charly): Should we change "displayedTexture" to "debuggedTexture" ?
            //                It would make more sense if we are able to show the
            //                debugged texture in its own viewport.
            /**
             * @brief The texture that will be displayed on screen. If no call to
             * @see debugTexture has been done, this is just a pointer to
             * @see m_finalTexture.
             */
            Texture* m_displayedTexture;

            std::unique_ptr<Texture> m_fancyTexture;
            std::map<std::string, Texture*> m_secondaryTextures;

            // FIXME(Charly): Scene class
            std::vector<std::shared_ptr<Light>> m_lights;

            bool m_renderQueuesUpToDate;

            std::vector<RenderObjectPtr> m_fancyRenderObjects;
            std::vector<RenderObjectPtr> m_debugRenderObjects;
            std::vector<RenderObjectPtr> m_xrayRenderObjects;
            std::vector<RenderObjectPtr> m_uiRenderObjects;

            // Simple quad mesh, used to render the final image
            std::unique_ptr<Mesh> m_quadMesh;

            bool m_drawDebug;           // Should we render debug stuff ?
            bool m_wireframe;           // Are we rendering in "real" wireframe mode
            bool m_postProcessEnabled;  // Should we do post processing ?

        private:
            // Qt has the nice idea to bind an fbo before giving you the opengl context,
            // this flag is used to save it (and render the final screen on it)
            int m_qtPlz;

            // Renderer timings data
            TimerData m_timerData;

            std::mutex m_renderMutex;

            // PICKING STUFF
            std::unique_ptr<FBO>     m_pickingFbo;
            std::unique_ptr<Texture> m_pickingTexture;

            // TODO(Charly): Check if this leads to some rendering / picking bugs
            // (because different depth textures would be written, and so on)
            std::unique_ptr<Texture>    m_depthTexture;

            std::vector<PickingQuery>   m_pickingQueries;
            std::vector<PickingQuery>   m_lastFramePickingQueries;
            std::vector<int>            m_pickingResults;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_HPP
