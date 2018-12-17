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
#include <Core/Utils/Timer.hpp>

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
}

namespace Ra {
namespace Engine {

  /**
   * Abstract renderer for the engine.
   * @see Radium Engine default rendering informations
   */
class RA_ENGINE_API Renderer {
  protected:
    using RenderObjectPtr = std::shared_ptr<RenderObject>;

  public:
    /**
     * Instrumentation structure that allow to extract timings from the rendering
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
     * Picking mode
     */
    enum PickingMode {
        RO = 0,    ///< Pick a mesh
        VERTEX,    ///< Pick a vertex of a mesh
        EDGE,      ///< Pick an edge of a mesh
        TRIANGLE,  ///< Pick a triangle of a mesh
        C_VERTEX,  ///< Picks all vertices of a mesh within a screen space circle
        C_EDGE,    ///< Picks all edges of a mesh within a screen space circle
        C_TRIANGLE ///< Picks all triangles of a mesh within a screen space circle
    };

    /**
     * Picking query
     */
    struct PickingQuery {
        Core::Vector2 m_screenCoords;
        Core::MouseButton::MouseButton m_button;
        PickingMode m_mode;
    };

    /**
     * Picking result
     */
    struct PickingResult {
        PickingMode m_mode;            // Picking mode of the query
        int m_roIdx;                   // Idx of the picked RO
        std::vector<int> m_vertexIdx;  // Idx of the picked vertex in the element, i.e. point's idx
                                       // OR idx in line or triangle
        std::vector<int> m_elementIdx; // Idx of the element, i.e. triangle for mesh, edge for lines
                                       // and -1 for points
        std::vector<int> m_edgeIdx;    // Idx of the opposite vertex in the triangle if mesh
        // Note: There is exactly one triplet for each querried pixel (thus there can be doublons
        // coming from pixels). Note: Beware that the same mesh vertex would also be picked for each
        // of its adjacent triangles.
    };

  public:
    /** Abstract rendere constructor
     *
     * could be called without openGL context.
     * Call initialize once the openGL rendering context is available before using the renderer
     */
    Renderer();

    virtual ~Renderer();

    // -=-=-=-=-=-=-=-=- FINAL -=-=-=-=-=-=-=-=- //

     /**
      * Extract the timings from las render
      */
    inline const TimerData& getTimerData() const { return m_timerData; }

    /**
     * Get the currently displayed texture
     */
    inline Texture* getDisplayTexture() const { return m_displayedTexture; }

    // Lock the renderer (for MT access)
    /**
     * Lock rendering. Usefull if there is multithread update of the rendering data
     */
    inline void lockRendering() { m_renderMutex.lock(); }

    /**
     * Unlock the rendering.
     */
    inline void unlockRendering() { m_renderMutex.unlock(); }

    /**
     * Toggle the fill/wireframe rendering mode
     */
    inline void toggleWireframe() { m_wireframe = !m_wireframe; }

    /**
     * set the fill/wireframe rendering mode
     * @param enabled true if rendering mode must be wireframe, false for fill render mode
     */
    inline void enableWireframe(bool enabled) { m_wireframe = enabled; }

    /**
     * Toggle debug rendering
     */
    inline void toggleDrawDebug() { m_drawDebug = !m_drawDebug; }

    /**
      * Set the debug rendering mode
      * @param enabled true if rendering mode must include debug objects, false else
      */
    inline void enableDebugDraw( bool enabled ) { m_drawDebug = enabled; }

    /**
     * set the post-process mode
     * @param enabled true if post processing must bve applied before display.
     */
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
    void render( const ViewingParameters& renderData );

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

     /**
      * Add a new picking query for the next rendering
      * @param query
      */
    inline void addPickingRequest( const PickingQuery& query ) {
        m_pickingQueries.push_back( query );
    }

    /**
     * Get the vector of picking results.
     * Results in the returned vector correspond to queries in the return vector by the function getPickingQueries.
     * @return Queries results
     */
    inline const std::vector<PickingResult>& getPickingResults() const { return m_pickingResults; }

    /**
    * Get the vector of picking queries.
    * Queries in the returned vector correspond to results in the return vector by the function getPickingResults.
    * @return Queries results
    */
    inline const std::vector<PickingQuery>& getPickingQueries() const {
        return m_lastFramePickingQueries;
    }

    inline void setMousePosition( const Core::Vector2& pos ) {
        m_mousePosition[0] = pos[0];
        m_mousePosition[1] = m_height - pos[1];
    }

    inline void setBrushRadius( Scalar brushRadius ) { m_brushRadius = brushRadius; }

    /// Tell if the renderer has an usable light.
    bool hasLight() const;

    // -=-=-=-=-=-=-=-=- VIRTUAL -=-=-=-=-=-=-=-=- //
    /** Add a light to the renderer.
      * may be overridden to filter the light or to specialize the way ligths are added to the
      * renderer ...
      * @param light
      */
    virtual void addLight( const Light* light );

    /**
     * Reload, recompile and relink all shaders and programmed internally used by the renderer.
     */
    virtual void reloadShaders();

    // TODO:    For now the drawn texture takes the whole viewport,
    //          maybe it could be great if we had a way to switch between
    //          the current "fullscreen" debug mode, and some kind of
    //          "windowed" mode (that would show the debugged texture in
    //          its own viewport, without hiding the final texture.)
    /**
     * @brief Change the texture that is displayed on screen.
     * Set m_displayedIsDepth to true if depth linearization is wanted
     *
     * @param texName The texture to display.
     */
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

    virtual std::unique_ptr<uchar[]> grabFrame(size_t &w, size_t &h) const;

  protected:

    /**
     * @brief initializeInternal
     * Initialize the renderer dependant resources.
     */
    virtual void initializeInternal() = 0;

    /**
     * resize the renderer dependent resources
     */
    virtual void resizeInternal() = 0;

    /**
     * Update the renderer dependent resources for the next frame
     * @param renderData
     */
    virtual void updateStepInternal( const ViewingParameters& renderData ) = 0;

    /**
     * @brief All the scene rendering magics basically happens here.
     *
     * @param renderData The basic data needed for the rendering :
     * Time elapsed since last frame, camera view matrix, camera projection matrix.
     */
    virtual void renderInternal( const ViewingParameters& renderData ) = 0;

    /**
     * @brief Do all post processing stuff. If you override this method,
     * be careful to fill @see m_fancyTexture since it is the texture that
     * will be displayed at the very end of the @see render method.
     *
     * @param renderData The basic data needed for the rendering :
     * Time elapsed since last frame, camera view matrix, camera projection matrix.
     */
    virtual void postProcessInternal( const ViewingParameters& renderData ) = 0;

    /**
     * @brief Add the debug layer with useful informations
     */
    virtual void debugInternal( const ViewingParameters& renderData ) = 0; // is viewingParameters useful ?

    /**
     * @brief Draw the UI data
     */
    virtual void uiInternal( const ViewingParameters& renderData ) = 0; // idem ?

  private:
    // 0.
    void saveExternalFBOInternal();

    // 1.
    void feedRenderQueuesInternal( const ViewingParameters& renderData );

    // 2.0
    void updateRenderObjectsInternal( const ViewingParameters& renderData );

    // 3.
    void splitRenderQueuesForPicking( const ViewingParameters& renderData );
    void splitRQ( const std::vector<RenderObjectPtr>& renderQueue,
                  std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking );
    void renderForPicking( const ViewingParameters& renderData,
                           const std::array<const ShaderProgram*, 4>& pickingShaders,
                           const std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking );

    void doPicking( const ViewingParameters& renderData );

    // 6.
    void drawScreenInternal();

    // 7.
    void notifyRenderObjectsRenderingInternal();

  protected:
    uint m_width { 0 };
    uint m_height { 0 };

    ShaderProgramManager* m_shaderMgr { nullptr };
    RenderObjectManager* m_roMgr { nullptr };

    //                It would make more sense if we are able to show the
    //                debugged texture in its own viewport.
    /**
     * @brief The texture that will be displayed on screen. If no call to
     * @see debugTexture has been done, this is just a pointer to
     * @see m_fancyTexture.
     */
    Texture* m_displayedTexture { nullptr };


    /// A renderer could define several LightManager (for instance, one for point light, one other
    /// for infinite light ...)
    std::vector<Ra::Engine::LightManager*> m_lightmanagers;

    bool m_renderQueuesUpToDate { false };

    std::vector<RenderObjectPtr> m_fancyRenderObjects;
    std::vector<RenderObjectPtr> m_debugRenderObjects;
    std::vector<RenderObjectPtr> m_xrayRenderObjects;
    std::vector<RenderObjectPtr> m_uiRenderObjects;

    // Simple quad mesh, used to render the final image
    std::unique_ptr<Mesh> m_quadMesh;

    bool m_drawDebug { true };          // Should we render debug stuff ?
    bool m_wireframe { false };          // Are we rendering in "real" wireframe mode
    bool m_postProcessEnabled { true }; // Should we do post processing ?

    // derived class could use the already created textures
    /// Depth texture : might be attached to the main framebuffer
    std::unique_ptr<Texture> m_depthTexture ;
    /// Final color texture : might be attached to the main framebuffer
    std::unique_ptr<Texture> m_fancyTexture;
    /// Textures exposed in the texture section box to be displayed.
    std::map<std::string, Texture*> m_secondaryTextures;

private:
    // Qt has the nice idea to bind an fbo before giving you the opengl context,
    // this flag is used to save it (and render the final screen on it)
    int m_qtPlz;
    int m_qtViewport[4];

    // Renderer timings data
    TimerData m_timerData;

    std::mutex m_renderMutex;

    // PICKING STUFF
    Ra::Core::Vector2 m_mousePosition;
    float m_brushRadius { 0 };
    std::unique_ptr<globjects::Framebuffer> m_pickingFbo;
    std::unique_ptr<Texture> m_pickingTexture;

    std::array<std::vector<RenderObjectPtr>, 4> m_fancyRenderObjectsPicking;
    std::array<std::vector<RenderObjectPtr>, 4> m_debugRenderObjectsPicking;
    std::array<std::vector<RenderObjectPtr>, 4> m_xrayRenderObjectsPicking;
    std::array<std::vector<RenderObjectPtr>, 4> m_uiRenderObjectsPicking;
    std::array<const ShaderProgram*, 4> m_pickingShaders;

    std::vector<PickingQuery> m_pickingQueries;
    std::vector<PickingQuery> m_lastFramePickingQueries;
    std::vector<PickingResult> m_pickingResults;

};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_HPP
