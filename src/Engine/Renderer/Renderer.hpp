#ifndef RADIUMENGINE_RENDERER_HPP
#define RADIUMENGINE_RENDERER_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

#include <Core/Math/Types.hpp>
#include <Core/Utils/Timer.hpp>
#include <GuiBase/Event/EventEnums.hpp>

namespace Ra {

namespace Engine {
class Camera;
class RenderObject;
class Light;
class ShaderProgram;
class ShaderProgramManager;
class TextureManager;
class RenderObjectManager;
class LightManager;
class Texture;
class Mesh;
struct ViewingParameters;
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

/**
 * Abstract Renderer for the Engine.
 * \see The Radium Engine default rendering information for more information
 *      about rendering with Radium.
 */
class RA_ENGINE_API Renderer {
  protected:
    using RenderObjectPtr = std::shared_ptr<RenderObject>;

  public:
    /**
     * Instrumentation structure that allow to extract timings from the rendering.
     */
    struct TimerData {
        Core::Utils::TimePoint renderStart;
        Core::Utils::TimePoint updateEnd;
        Core::Utils::TimePoint feedRenderQueuesEnd;
        Core::Utils::TimePoint mainRenderEnd;
        Core::Utils::TimePoint postProcessEnd;
        Core::Utils::TimePoint renderEnd;
    };

    /**
     * Picking mode.
     */
    enum PickingMode {
        RO = 0,    ///< Pick a mesh.
        VERTEX,    ///< Pick a vertex of a mesh.
        EDGE,      ///< Pick an edge of a mesh.
        TRIANGLE,  ///< Pick a triangle of a mesh.
        C_VERTEX,  ///< Picks all vertices of a mesh within a screen space circle.
        C_EDGE,    ///< Picks all edges of a mesh within a screen space circle.
        C_TRIANGLE ///< Picks all triangles of a mesh within a screen space circle.
    };

    /**
     * Picking query.
     */
    struct PickingQuery {
        Core::Vector2 m_screenCoords;               ///< Request pixel.
        GuiBase::MouseButton::MouseButton m_button; ///< Request mouse button.
        PickingMode m_mode;                         ///< Request type.
    };

    /**
     * Picking result.
     */
    struct PickingResult {
        PickingMode m_mode; ///< Picking mode of the query.
        int m_roIdx;        ///< Idx of the picked RO.
        /**
         * \note There is exactly one triplet for each querried pixel (thus
         *       there can be doublons coming from adjacent pixels).
         * \note Beware that the same mesh vertex would also be picked for each
         *       of its adjacent triangles.
         */
        /// \{
        std::vector<int> m_vertexIdx;  ///< Idx of the picked vertex in the element,
                                       ///< i.e.\ point's idx OR idx in line or triangle.
        std::vector<int> m_elementIdx; ///< Idx of the element, i.e.\ triangle for mesh,
                                       ///< edge for lines and -1 for points.
        std::vector<int> m_edgeIdx;    ///< Idx of the opposite vertex in the
                                       ///< triangle if mesh, -1 otherwise.
        /// \}
    };

  public:
    /**
     * Abstract Renderer constructor.
     *
     * \note Can be called without OpenGL context, but initialize must then be
     *       called once the OpenGL rendering context is available
     *       before using the renderer.
     */
    Renderer();

    virtual ~Renderer();

    /**
     * Return the name of the Renderer.
     */
    virtual std::string getRendererName() const = 0;

    /**
     * Initialize the Renderer internal data.
     */
    void initialize( uint width, uint height );

    /**
     * \brief Resize the viewport and all the screen Textures, FBOs.
     * This function must be overrided as soon as some FBO or screensized
     * Texture is used (since the default implementation just resizes its
     * own FBOs / Textures).
     */
    void resize( uint width, uint height );

    /**
     * \brief Tell the Renderer it needs to render.
     * This method does the following steps :
     * <ol>
     *   <li>fill render queues </li>
     *   <li>update data </li>
     *   <li>perform picking </li>
     *   <li>render </li>
     *   <li>perform post process </li>
     *   <li>render the final texture in the right framebuffer</li>
     * </ol>
     *
     * \param renderData The basic data needed for the rendering:
     *        time elapsed since last frame, camera view matrix, camera projection matrix.
     *
     * \note What "render in the right buffer" means, is that, for example,
     *       when using QOpenGLWidget, Qt binds its own framebuffer before calling
     *       updateGL() method.
     *       So, render() takes that into account by saving an eventual bound
     *       framebuffer, and restores it before drawing the last final texture.
     *       If no framebuffer was bound, it draws into GL_BACK.
     */
    void render( const ViewingParameters& renderData );

    /**
     * Lock rendering. Usefull if there is multithread update of the rendering data.
     */
    inline void lockRendering() { m_renderMutex.lock(); }

    /**
     * Unlock the rendering.
     */
    inline void unlockRendering() { m_renderMutex.unlock(); }

    /**
     * Extract the timings from last render.
     */
    inline const TimerData& getTimerData() const { return m_timerData; }

    /**
     * Tell if the renderer has a usable Light.
     */
    bool hasLight() const;

    /**
     * Add a light to the Renderer.
     * May be overridden to filter the Light or to specialize the way Ligths are
     * added to the Renderer.
     */
    virtual void addLight( const Light* light );

    /**
     * Reload, recompile and relink all shaders and programmed internally used by the renderer.
     */
    virtual void reloadShaders();

    /// \name Textures
    /// \{

    // TODO: For now the drawn texture takes the whole viewport,
    //       maybe it could be great if we had a way to switch between
    //       the current "fullscreen" debug mode, and some kind of
    //       "windowed" mode (that would show the debugged texture in
    //       its own viewport, without hiding the final texture.)
    /**
     * Change the Texture that is displayed on screen.
     * \note Set m_displayedIsDepth to true if depth linearization is wanted.
     */
    virtual void displayTexture( const std::string& texName );

    /**
     * Return the currently displayed Texture.
     */
    inline Texture* getDisplayTexture() const { return m_displayedTexture; }

    /**
     * Return the names of Textures available in the Renderer.
     */
    virtual std::vector<std::string> getAvailableTextures() const;

    /**
     * Return the rendered frame as raw data, along with its size.
     */
    virtual std::unique_ptr<uchar[]> grabFrame( size_t& w, size_t& h ) const;
    /// \}

    /// \name Rendering options
    /// \{

    /**
     * Toggle the fill/wireframe rendering mode.
     */
    inline void toggleWireframe() { m_wireframe = !m_wireframe; }

    /**
     * Toggle the fill/wireframe rendering mode.
     */
    inline void enableWireframe( bool enabled ) { m_wireframe = enabled; }

    /**
     * Toggle on/off drawing Debug meshes.
     */
    inline void toggleDrawDebug() { m_drawDebug = !m_drawDebug; }

    /**
     * Toggle on/off drawing Debug meshes.
     */
    inline void enableDebugDraw( bool enabled ) { m_drawDebug = enabled; }

    /**
     * Toggle on/off post-process.
     */
    inline void enablePostProcess( bool enabled ) { m_postProcessEnabled = enabled; }
    /// \}

    /// \name Picking
    /// \{

    /**
     * Add the given PickingQuery for treatment at the next frame.
     */
    inline void addPickingRequest( const PickingQuery& query ) {
        m_pickingQueries.push_back( query );
    }

    /**
     * Return the results of all the PickingQueries processed during the last frame.
     */
    inline const std::vector<PickingResult>& getPickingResults() const { return m_pickingResults; }

    /**
     * Return the list of PickingQueries processed during the last frame.
     */
    inline const std::vector<PickingQuery>& getPickingQueries() const {
        return m_lastFramePickingQueries;
    }

    /**
     * Set the Mouse position for tracking.
     */
    inline void setMousePosition( const Core::Vector2& pos ) {
        m_mousePosition[0] = pos[0];
        m_mousePosition[1] = m_height - pos[1];
    }

    /**
     * Set the circle brush radius.
     */
    inline void setBrushRadius( Scalar brushRadius ) { m_brushRadius = brushRadius; }
    /// \}

  protected:
    /**
     * Initialize the Renderer resources.
     */
    virtual void initializeInternal() = 0;

    /**
     * Resize the Renderer resources.
     */
    virtual void resizeInternal() = 0;

    /**
     * Update the view-dependant resources for the next frame.
     */
    virtual void updateStepInternal( const ViewingParameters& renderData ) = 0;

    /**
     * \brief All the scene rendering magics basically happens here.
     *
     * \param renderData The basic data needed for the rendering:
     *        time elapsed since last frame, camera view matrix, camera projection matrix.
     */
    virtual void renderInternal( const ViewingParameters& renderData ) = 0;

    /**
     * \brief Do all post processing stuff.
     *
     * If you override this method, be careful to fill m_fancyTexture since it is
     * the texture that will be displayed at the very end of the render() method.
     *
     * \param renderData The basic data needed for the rendering:
     *        time elapsed since last frame, camera view matrix, camera projection matrix.
     */
    virtual void postProcessInternal( const ViewingParameters& renderData ) = 0;

    /**
     * Add the debug layer with useful informations.
     */
    // FIXME: is viewingParameters useful ?
    virtual void debugInternal( const ViewingParameters& renderData ) = 0;

    /**
     * Add the ui layer with useful informations.
     */
    // FIXME: is viewingParameters useful ?
    virtual void uiInternal( const ViewingParameters& renderData ) = 0;

  private:
    // 0.
    /**
     * Save current viewport and FBO and resize the viewport.
     */
    void saveExternalFBOInternal();

    // 1.
    /**
     * Fill the render queues from the scene's RenderObjects.
     */
    void feedRenderQueuesInternal( const ViewingParameters& renderData );

    // 2.
    /**
     * Update RenderObject's data on OpenGL side.
     */
    void updateRenderObjectsInternal( const ViewingParameters& renderData );

    // 3.
    /**
     * Call splitRQ on each render queue.
     */
    void splitRenderQueuesForPicking( const ViewingParameters& renderData );

    /**
     * Split the given render queue according to the RenderObject's MeshRenderMode.
     */
    void splitRQ( const std::vector<RenderObjectPtr>& renderQueue,
                  std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking );

    /**
     * Render the given render queue for picking.
     * \note each sub-queue is rendered with the corresponding picking Shader.
     */
    void renderForPicking( const ViewingParameters& renderData,
                           const std::array<const ShaderProgram*, 4>& pickingShaders,
                           const std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking );

    /**
     * Process all picking requests.
     */
    void doPicking( const ViewingParameters& renderData );

    // 6.
    /**
     * Display the final Texture on screen.
     * \note Also displays the screen-space picking circle on top of it.
     */
    void drawScreenInternal();

    // 7.
    /**
     * Specify each rendered RenderObject they have been rendered.
     */
    void notifyRenderObjectsRenderingInternal();

  protected:
    /// The Renderer width.
    uint m_width{0};

    /// The Renderer height.
    uint m_height{0};

    /// The ShaderProgram Manager.
    ShaderProgramManager* m_shaderMgr{nullptr};

    /// The RenderObject Manager.
    RenderObjectManager* m_roMgr{nullptr};

    /// The list of LightManagers.
    std::vector<Ra::Engine::LightManager*> m_lightmanagers;

    /** \name Textures
     * \note Derived class can use the already created textures.
     */
    /// \{

    /**
     * Raw pointer to the texture that will be displayed on screen.
     * \note If no call to debugTexture has been done, this is just
     *       a pointer to m_fancyTexture.
     */
    // FIXME: It would make more sense if we are able to show the debugged
    //        texture in its own viewport.
    Texture* m_displayedTexture{nullptr};

    /// The depth texture, might be attached to the main framebuffer.
    std::unique_ptr<Texture> m_depthTexture;

    /// The final colored texture, might be attached to the main framebuffer.
    std::unique_ptr<Texture> m_fancyTexture;

    /// All the Textures used by the Renderer.
    std::map<std::string, Texture*> m_secondaryTextures;
    /// \}

    /// \name Render queues.
    /// \{

    /// Whether the RenderQueues are fed.
    bool m_renderQueuesUpToDate{false};

    /// The render queue for non x-rayed objects in the scene.
    std::vector<RenderObjectPtr> m_fancyRenderObjects;

    /// The render queue for debug objects.
    std::vector<RenderObjectPtr> m_debugRenderObjects;

    /// The render queue for xray-ed objects of the scene.
    std::vector<RenderObjectPtr> m_xrayRenderObjects;

    /// The render queue for ui objects.
    std::vector<RenderObjectPtr> m_uiRenderObjects;
    /// \}

    /// Simple quad mesh, used to render the final image.
    std::unique_ptr<Mesh> m_quadMesh;

    /// Whether to render debug objects.
    bool m_drawDebug{true};

    /// Whether to render in wireframe mode.
    bool m_wireframe{false};

    /// Whether to perform post-process.
    bool m_postProcessEnabled{true};

  private:
    // Qt has the nice idea to bind an fbo before giving you the opengl context,
    // this flag is used to save it (and render the final screen on it).
    int m_qtPlz;
    int m_qtViewport[4];

    /// Renderer timings data.
    TimerData m_timerData;

    /// Guards the Renderer against thread concurrency.
    std::mutex m_renderMutex;

    /// \name Picking stuff
    /// \{

    /// Current position of the mouse.
    Ra::Core::Vector2 m_mousePosition;

    /// Radius of the picking circle brush.
    float m_brushRadius{0};

    /// Picking FBO.
    std::unique_ptr<globjects::Framebuffer> m_pickingFbo;

    /// Picking Texture.
    std::unique_ptr<Texture> m_pickingTexture;

    /// The render queues for picking non x-rayed objects in the scene.
    std::array<std::vector<RenderObjectPtr>, 4> m_fancyRenderObjectsPicking;

    /// The render queues for picking x-rayed objects in the scene.
    std::array<std::vector<RenderObjectPtr>, 4> m_xrayRenderObjectsPicking;

    /// The render queues for picking debug objects in the scene.
    std::array<std::vector<RenderObjectPtr>, 4> m_debugRenderObjectsPicking;

    /// The render queues for picking ui objects in the scene.
    std::array<std::vector<RenderObjectPtr>, 4> m_uiRenderObjectsPicking;

    /// Picking Shaders.
    std::array<const ShaderProgram*, 4> m_pickingShaders;

    /// Picking queries for the next frame.
    std::vector<PickingQuery> m_pickingQueries;

    /// Picking queries from the last frame.
    std::vector<PickingQuery> m_lastFramePickingQueries;

    /// Picking resulsts from the last frame.
    std::vector<PickingResult> m_pickingResults;
    /// \}
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_HPP
