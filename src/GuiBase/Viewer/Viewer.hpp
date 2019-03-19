#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <atomic>
#include <memory>

#include <QWindow>

#include <QThread>

#include <Core/CoreMacros.hpp>
#include <Core/Types.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

// Forward declarations
class QOpenGLContext;
class QSurfaceFormat;

namespace Ra {
namespace Core {
struct KeyEvent;
struct MouseEvent;
} // namespace Core
} // namespace Ra

namespace Ra {
namespace Gui {
class CameraInterface;
class GizmoManager;
class PickingManager;
} // namespace Gui
} // namespace Ra

namespace Ra {
namespace Gui {

/**
 * \brief The Viewer is the main display class.
 *
 * It could be used as an independant window or can be set as a central widget
 * on a more complex gui by using the adapter from QWindow to QWidget.
 * To do that, the following code could be used :
 * \code
 *     m_viewer = new Ra::Gui::Viewer();
 *     QWidget * viewerwidget = QWidget::createWindowContainer(m_viewer);
 *     setCentralWidget(viewerwidget);
 * \endcode
 *
 * Whatever its usage (QWindow or QWidget) the Viewer has the same funtionalities.
 * Its acts as a bridge between the interface, the engine and the renderer.
 * Among its responsibilities are :
 * - Owning the renderer and camera, and managing their lifetime.
 * - Setting up the renderer and camera by keeping it informed of interfaces changes
 *   (e.g.\ resize).
 * - Catching user interactions (mouse clicks) at the lowest level and forward
 *   them to the camera and the rest of the application.
 * - Expose the asynchronous rendering interface.
 */
class RA_GUIBASE_API Viewer : public QWindow {
    Q_OBJECT

  public:
    explicit Viewer( QScreen* screen = nullptr );

    ~Viewer() override;

    /**
     * Create gizmos.
     */
    void createGizmoManager();

    /**
     * Make the OpenGL context associated with the viewer the current context.
     */
    void makeCurrent();

    /**
     * Reset the current OpenGL context that is no more the one associated with the viewer.
     */
    void doneCurrent();

    /// \name Accessors
    /// \{

    /**
     * Return the OpenGLContext.
     */
    QOpenGLContext* getContext() const { return m_context.get(); }

    /**
     * Return true if the OpenGLContext has been initialized.
     */
    bool isOpenGlInitialized() const { return m_glInitStatus; }

    /**
     * Return the CameraInterace.
     */
    CameraInterface* getCameraInterface();

    /**
     * Return the GizmoManager.
     */
    GizmoManager* getGizmoManager();

    /**
     * Return the active Renderer.
     */
    const Engine::Renderer* getRenderer() const;

    /**
     * Return the active Renderer.
     */
    Engine::Renderer* getRenderer();

    /**
     * Return the PickingManager.
     */
    PickingManager* getPickingManager();
    /// \}

    /// \name Rendering management
    /// \{

    /**
     * Start rendering (potentially asynchronously in a separate thread).
     */
    void startRendering( const Scalar dt );

    /**
     * Blocks until rendering is finished.
     */
    void waitForRendering();
    /// \}

    /**
     * Emits signals corresponding to picking requests so that listeners can
     * do their job.
     */
    void processPicking();

    /**
     * Moves the Camera so that the whole scene is visible.
     */
    void fitCameraToScene( const Core::Aabb& sceneAabb );

    /**
     * Returns the names of the different registered Renderers.
     */
    std::vector<std::string> getRenderersName() const;

    /**
     * Write the current frame as an image. Supports either BMP or PNG file names.
     */
    void grabFrame( const std::string& filename );

    /**
     * Activates printing stat info to the error output after each glbinding call.
     */
    void enableDebug();

  signals:
    /**
     * Emitted when GL context is ready. We except call to addRenderer() here.
     */
    void glInitialized();

    /**
     * Emitted when the Renderer is correctly initialized.
     */
    void rendererReady();

    /**
     * Emitted when the result of a manipulation picking is known.
     */
    void leftClickPicking( int id );

    /**
     * Emitted when the resut of a selection picking is known.
     */
    void rightClickPicking( const Ra::Engine::Renderer::PickingResult& result );

    /**
     * Emitted when the corresponding key is released (see keyReleaseEvent() and
     * KeyMappingManager).
     */
    void toggleBrushPicking( bool on );

  public slots:
    /**
     * Tell the Renderer to reload all Shaders.
     */
    void reloadShaders();

    /**
     * Set the final display Texture.
     */
    void displayTexture( const QString& tex );

    /**
     * Set the current Renderer.
     */
    bool changeRenderer( int index );

    /**
     * Toggle the Renderer post-process.
     */
    void enablePostProcess( int enabled );

    /**
     * Toggle drawing debug objects.
     */
    void enableDebugDraw( int enabled );

    /**
     * Add a Renderer and return its index.
     * \param e unique_ptr to the new Renderer.
     *
     * Example usage:
     * \code
     * int rendererId = addRenderer(new MyRenderer(width(), height()));
     * changeRenderer(rendererId);
     * getRenderer()->initialize();
     * auto light = Ra::Core::make_shared<Engine::DirectionalLight>();
     * m_camera->attachLight( light );
     * \endcode
     */
    int addRenderer( std::shared_ptr<Engine::Renderer> e );

  private slots:
    /**
     * These slots are connected to the base class signals to properly handle
     * concurrent access to the renderer.
     */
    /// \{
    void onAboutToCompose();
    void onAboutToResize();
    void onFrameSwapped();
    void onResized();
    /// \}

  protected:
    /**
     * Initialize Renderer internal state + configure Lights.
     */
    void intializeRenderer( Engine::Renderer* renderer );

    //
    // OpenGL stuff.
    // Not herited, defined here in the same way QOpenGLWidget defines them.
    //

    /**
     * Initialize openGL. Called on by the first "show" call to the main window.
     * \warning This function is NOT reentrant, and may behave incorrectly
     *          if called at the same time than intializeRenderer().
     */
    virtual void initializeGL();

    /**
     * Resize the viewport and the Camera. Called by the resize event.
     */
    virtual void resizeGL( int width, int height );

    /** \name Qt input events
     * \note We intercept the mouse events in this widget to get the coordinates
     *       of the mouse in screen space.
     */
    /// \{
    void resizeEvent( QResizeEvent* event ) override;

    void keyPressEvent( QKeyEvent* event ) override;

    void keyReleaseEvent( QKeyEvent* event ) override;

    void mousePressEvent( QMouseEvent* event ) override;

    void mouseReleaseEvent( QMouseEvent* event ) override;

    void mouseMoveEvent( QMouseEvent* event ) override;

    void wheelEvent( QWheelEvent* event ) override;

    void showEvent( QShowEvent* ev ) override;

    void exposeEvent( QExposeEvent* ev ) override;
    /// \}

    /**
     * Return the Picking mode.
     */
    Engine::Renderer::PickingMode getPickingMode() const;

  public:
    Scalar m_dt;

  protected:
    /// OpenglContext used with this widget.
    std::unique_ptr<QOpenGLContext> m_context;

    /// The list of available Renderers.
    std::vector<std::shared_ptr<Engine::Renderer>> m_renderers;

    /// The current Renderer.
    Engine::Renderer* m_currentRenderer;

    /// The PickingManager.
    PickingManager* m_pickingManager;

    /// Whether the picking circle is enabled.
    bool m_isBrushPickingEnabled;

    /// The radius of the picking circle.
    float m_brushRadius;

    /// The CameraInterface.
    std::unique_ptr<CameraInterface> m_camera;

    /// The GizmoManager.
    GizmoManager* m_gizmoManager;

#ifdef RADIUM_MULTITHREAD_RENDERING
    // TODO are we really using this ? Remove if we do not plan to do multi thread rendering.
    // We have to use a QThread for MT rendering.
    /// Thread in which rendering is done.
    [[deprecated]] QThread* m_renderThread = nullptr;
#endif

    /// OpenGL initialization status.
    std::atomic_bool m_glInitStatus;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
