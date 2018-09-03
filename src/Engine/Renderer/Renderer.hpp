#ifndef RADIUMENGINE_RENDERER_HPP
#define RADIUMENGINE_RENDERER_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

#include <Core/Event/EventEnums.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Time/Timer.hpp>

namespace Ra {
namespace Core {
struct MouseEvent;
struct KeyEvent;
} // namespace Core

namespace Engine {
class Camera;
class RenderObject;
class Light;
class Mesh;
class ShaderProgram;
class ShaderProgramManager;
class Texture;
class TextureManager;
class RenderObjectManager;
class LightManager;
} // namespace Engine

namespace Asset {
class FileData;
} // namespace Asset
} // namespace Ra

namespace globjects {
class Framebuffer;
} // namespace globjects

namespace Ra {
namespace Engine {

/// Viewpoint data.
struct RA_ENGINE_API RenderData {
    Core::Matrix4 viewMatrix;
    Core::Matrix4 projMatrix;
    Scalar dt;
};

/// The RendererClass is the generic class for renderers.
class RA_ENGINE_API Renderer {
  protected:
    using RenderObjectPtr = std::shared_ptr<RenderObject>;

  public:
    /// Timer data used for timings, ordered per event order.
    struct TimerData {
        Core::Timer::TimePoint renderStart;         ///< When did the rendering start.
        Core::Timer::TimePoint updateEnd;           ///< When did the scene update end.
        Core::Timer::TimePoint feedRenderQueuesEnd; ///< When did the RenderQueue got fed.
        Core::Timer::TimePoint mainRenderEnd;       ///< When did the main rendering end.
        Core::Timer::TimePoint postProcessEnd;      ///< When did the postprossed end.
        Core::Timer::TimePoint renderEnd;           ///< When did the rendering end.
    };

    /// Used to indicate the type of picking.
    enum PickingMode {
        RO = 0,    ///< Pick a mesh.
        VERTEX,    ///< Pick a vertex of a mesh.
        EDGE,      ///< Pick an edge of a mesh.
        TRIANGLE,  ///< Pick a triangle of a mesh.
        C_VERTEX,  ///< Picks all vertices of a mesh within a screen space circle.
        C_EDGE,    ///< Picks all edges of a mesh within a screen space circle.
        C_TRIANGLE ///< Picks all triangles of a mesh within a screen space circle.
    };

    /// Picking request data.
    struct PickingQuery {
        Core::Vector2 m_screenCoords;            ///< Request pixel.
        Core::MouseButton::MouseButton m_button; ///< Request mouse button.
        PickingMode m_mode;                      ///< Request type.
    };

    /// Stores data about the result of a picking.
    struct PickingResult {
        PickingMode m_mode;            ///< Picking type.
        int m_roIdx;                   ///< Idx of the picked RO.
        std::vector<int> m_vertexIdx;  ///< Idx of the picked vertex in the element,
                                       ///< i.e. point's idx OR idx in line or triangle.
        std::vector<int> m_elementIdx; ///< Idx of the element, i.e. triangle idx for meshes,
                                       ///< edge idx for lines and -1 for point coulds.
        std::vector<int> m_edgeIdx;    ///< Idx of the opposite vertex in the triangle if mesh.
        /// \note There is exactly one triplet for each querried pixel (thus
        /// there can be doublons coming from pixels).
        /// \note Beware that the same mesh vertex would also be picked for each
        /// of its adjacent triangles.
    };

  public:
    Renderer();

    virtual ~Renderer();

    /// Return the timings data.
    inline const TimerData& getTimerData() const { return m_timerData; }

    /// Return the Texture displayed on the screen.
    inline Texture* getDisplayTexture() const { return m_displayedTexture; }

    /// Lock the Renderer to guard it against thread concurrency.
    inline void lockRendering() { m_renderMutex.lock(); }

    /// Unlock the Renderer.
    inline void unlockRendering() { m_renderMutex.unlock(); }

    /// Toggle on/off displaying meshes in wireframe mode.
    inline void setWireframe( bool wireframe ) { m_wireframe = wireframe; }

    /// Toggle on/off displaying meshes in wireframe mode.
    inline void toggleWireframe() { m_wireframe = !m_wireframe; }

    /// Toggle on/off rendreing post-process.
    inline void enablePostProcess( bool enabled ) { m_postProcessEnabled = enabled; }

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
     * @brief Initialize renderer
     */
    void initialize( uint width, uint height );

    /**
     * @brief Resize the viewport and all the screen textures, fbos.
     * This function must be overrided as soon as some FBO or screensized
     * texture is used (since the default implementation just resizes its
     * own fbos / textures)
     *
     * @param width The new viewport width
     * @param height The new viewport height
     */
    void resize( uint width, uint height );

    /// Add the given picking request for treatment at the next frame.
    inline void addPickingRequest( const PickingQuery& query ) {
        m_pickingQueries.push_back( query );
    }

    /// Return all the picking requests processed during the last frame.
    inline const std::vector<PickingQuery>& getPickingQueries() const {
        return m_lastFramePickingQueries;
    }

    /// Return the results of all the picking requests processed during the last frame.
    inline const std::vector<PickingResult>& getPickingResults() const { return m_pickingResults; }

    /// Set the Mouse position for tracking.
    inline virtual void setMousePosition( const Core::Vector2& pos ) final {
        m_mousePosition[0] = pos[0];
        m_mousePosition[1] = m_height - pos[1];
    }

    /// Set the circle brush radius.
    inline virtual void setBrushRadius( Scalar brushRadius ) final { m_brushRadius = brushRadius; }

    /// Toggle on/off rendering of Debug objects.
    inline void enableDebugDraw( bool enabled ) { m_drawDebug = enabled; }

    /// Toggle on/off rendering of Debug objects.
    inline void toggleDrawDebug() { m_drawDebug = !m_drawDebug; }

    // FIXED : lights must be handled by the renderer as they are the reason to have different
    // renderers
    //                How to do this ?
    // FIXED : use a light manager (Implement the one you need)
    /** Add a light to the renderer.
     * may be overriden to filter the light or to specialize the way ligths are added to the
     * renderer ...
     * @param light
     */
    virtual void addLight( const Light* light );

    /// Tell if the renderer has an usable light.
    bool hasLight() const;

    /// Reload the Shaders.
    virtual void reloadShaders();

    /**
     * @brief Change the texture that is displayed on screen.
     * Set m_displayedIsDepth to true if depth linearization is wanted
     *
     * @param texName The texture to display.
     */
    // FIXME(Charly): For now the drawn texture takes the whole viewport,
    //                maybe it could be great if we had a way to switch between
    //                the current "fullscreen" debug mode, and some kind of
    //                "windowed" mode (that would show the debugged texture in
    //                its own viewport, without hiding the final texture.)
    virtual void displayTexture( const std::string& texName );

    /**
     * @brief Return the names of renderer available textures
     * @return A vector of strings, containing the name of the different textures
     */
    virtual std::vector<std::string> getAvailableTextures() const;

    /**
     * @brief Get the name of the renderer, e.g to be displayed in the UI
     * @return
     */
    virtual std::string getRendererName() const = 0;

    /// Return the rendered frame as raw data, along with its size.
    virtual uchar* grabFrame( uint& w, uint& h ) const;

  protected:
    /**
     * @brief initializeInternal
     */
    virtual void initializeInternal() = 0;

    /**
     * @brief resizeInternal
     */
    virtual void resizeInternal() = 0;

    // 2.1
    /**
     * @brief updateStepInternal
     */
    virtual void updateStepInternal( const RenderData& renderData ) = 0;

    // 4.
    /**
     * @brief All the scene rendering magics basically happens here.
     *
     * @param renderData The basic data needed for the rendering :
     * Time elapsed since last frame, camera view matrix, camera projection matrix.
     */
    virtual void renderInternal( const RenderData& renderData ) = 0;

    // 5.
    /**
     * @brief Do all post processing stuff. If you override this method,
     * be careful to fill @see m_fancyTexture since it is the texture that
     * will be displayed at the very end of the @see render method.
     *
     * @param renderData The basic data needed for the rendering :
     * Time elapsed since last frame, camera view matrix, camera projection matrix.
     */
    virtual void postProcessInternal( const RenderData& renderData ) = 0;

    /**
     * @brief Add the debug layer with useful informations
     */
    virtual void debugInternal( const RenderData& renderData ) = 0; // is renderData useful ?

    /**
     * @brief Draw the UI data
     */
    virtual void uiInternal( const RenderData& renderData ) = 0; // idem ?

  private:
    // 0.
    /**
     * @brief updateStepInternal
     */
    void saveExternalFBOInternal();

    // 1.
    /**
     * @brief feedRenderQueuesInternal
     */
    void feedRenderQueuesInternal( const RenderData& renderData );

    // 2.0
    /**
     * @brief updateRenderObjectsInternal
     */
    void updateRenderObjectsInternal( const RenderData& renderData );

    // 3.
    /// Split all the render queues by Mesh type.
    void splitRenderQueuesForPicking( const RenderData& renderData );

    /// Split the given render queue by Mesh type.
    void splitRQ( const std::vector<RenderObjectPtr>& renderQueue,
                  std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking );

    /// Render the given render queue with the given picking shader.
    void renderForPicking( const RenderData& renderData,
                           const std::array<const ShaderProgram*, 4>& pickingShaders,
                           const std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking );

    /// Process all the picking requests.
    void doPicking( const RenderData& renderData );

    // 6.
    /**
     * @brief drawScreenInternal
     */
    void drawScreenInternal();

    // 7.
    /**
     * @brief notifyRenderObjectsRenderingInternal
     */
    void notifyRenderObjectsRenderingInternal();

  protected:
    uint m_width;  ///< The Renderer width.
    uint m_height; ///< The Renderer height.

    ShaderProgramManager* m_shaderMgr; ///< The ShaderProgram Manager.
    RenderObjectManager* m_roMgr;      ///< The RenderObject Manager.

    // FIXME(Charly): Should we change "displayedTexture" to "debuggedTexture" ?
    //                It would make more sense if we are able to show the
    //                debugged texture in its own viewport.
    /**
     * @brief The texture that will be displayed on screen. If no call to
     * @see debugTexture has been done, this is just a pointer to
     * @see m_fancyTexture.
     */
    Texture* m_displayedTexture;

    /// The final rendering texture.
    std::unique_ptr<Texture> m_fancyTexture;

    /// All the other textures used for rendering.
    std::map<std::string, Texture*> m_secondaryTextures;

    /// A renderer could define several LightManager (for instance, one for point light, one other
    /// for infinite light ...)
    std::vector<Ra::Engine::LightManager*> m_lightmanagers;

    /// Whether the RenderQueues are fed.
    bool m_renderQueuesUpToDate;

    /// The render queue for non x-rayed objects in the scene.
    std::vector<RenderObjectPtr> m_fancyRenderObjects;

    /// The render queue for xray-ed objects of the scene.
    std::vector<RenderObjectPtr> m_xrayRenderObjects;

    /// The render queue for debug objects.
    std::vector<RenderObjectPtr> m_debugRenderObjects;

    /// The render queue for UI objects.
    std::vector<RenderObjectPtr> m_uiRenderObjects;

    /// Simple quad mesh, used to render the final image.
    std::unique_ptr<Mesh> m_quadMesh;

    /// Whether to render debug objects.
    bool m_drawDebug;

    /// Whether to render the scene objects in wireframe mode.
    bool m_wireframe;

    /// Whether to perform post-process.
    bool m_postProcessEnabled;

  private:
    // Qt has the nice idea to bind an fbo before giving you the opengl context,
    // this flag is used to save it (and render the final screen on it)
    int m_qtPlz;
    int m_qtViewport[4];

    /// Renderer timings data.
    TimerData m_timerData;

    /// Guards the Renderer against thread concurrency.
    std::mutex m_renderMutex;

    /// Current position of the mouse.
    Ra::Core::Vector2 m_mousePosition;

    /// Radius of the circle brush.
    float m_brushRadius;

    /// Picking FBO.
    std::unique_ptr<globjects::Framebuffer> m_pickingFbo;

    /// Picking Texture.
    std::unique_ptr<Texture> m_pickingTexture;

    /// The render queues for picking non x-rayed objects in the scene.
    std::array<std::vector<RenderObjectPtr>, 4> m_fancyRenderObjectsPicking;

    /// The render queues for picking x-rayed objects in the scene.
    std::array<std::vector<RenderObjectPtr>, 4> m_xrayRenderObjectsPicking;

    /// The render queues for picking debug objects.
    std::array<std::vector<RenderObjectPtr>, 4> m_debugRenderObjectsPicking;

    /// The render queues for picking UI objects.
    std::array<std::vector<RenderObjectPtr>, 4> m_uiRenderObjectsPicking;

    /// Picking Shaders.
    std::array<const ShaderProgram*, 4> m_pickingShaders;

    /// Picking queries for the next frame.
    std::vector<PickingQuery> m_pickingQueries;

    /// Picking queries from the last frame.
    std::vector<PickingQuery> m_lastFramePickingQueries;

    /// Picking resulsts from the last frame.
    std::vector<PickingResult> m_pickingResults;

    /// Depth Texture.
    std::unique_ptr<Texture> m_depthTexture;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_HPP
