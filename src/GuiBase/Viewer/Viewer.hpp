#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <atomic>
#include <memory>

#include <QWindow>

#include <QThread>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>

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

/// The Viewer is the main display class. It could be used as an independant window or
/// can be set as a central widget on a more complex gui by using the adapter fro QWindow to QWidget
/// To do that, the following code could be used :
/// \code{.cpp}
///     m_viewer = new Ra::Gui::Viewer();
///     QWidget * viewerwidget = QWidget::createWindowContainer(m_viewer);
///     setCentralWidget(viewerwidget);
/// \endcode
/// Whatever its usage (QWindow or QWidget) the Viewer has the same funtionalities.
/// Its acts as a bridge between the interface, the engine and the renderer
/// Among its responsibilities are :
/// * Owning the renderer and camera, and managing their lifetime.
/// * setting up the renderer and camera by keeping it informed of interfaces changes
///  (e.g. resize).
/// * catching user interaction (mouse clicks) at the lowest level and forward it to
/// the camera and the rest of the application
/// * Expose the asynchronous rendering interface
class RA_GUIBASE_API Viewer : public QWindow {
    Q_OBJECT

  public:
    explicit Viewer( QScreen* screen = nullptr );

    virtual ~Viewer();

    /// Create the gizmos.
    void createGizmoManager();

    /**
     * Make the OpenGL context associated with the viewer the current context.
     */
    void makeCurrent();

    /**
     * Reset the current OpenGL context that is no more the one associated with the viewer.
     */
    void doneCurrent();

    //
    // Accessors
    //

    /// Return the OpenGLContext.
    QOpenGLContext* getContext() const { return m_context.get(); }

    /// Return true if the OpenGLContext has been initialized.
    bool isOpenGlInitialized() const { return m_glInitStatus; }

    /// Access to camera interface.
    CameraInterface* getCameraInterface();

    /// Access to gizmo manager.
    GizmoManager* getGizmoManager();

    /// Read-only access to renderer.
    const Engine::Renderer* getRenderer() const;

    /// Read-write access to renderer.
    Engine::Renderer* getRenderer();

    /// Access to the feature picking manager.
    PickingManager* getPickingManager();

    //
    // Rendering management
    //

    /// Start rendering (potentially asynchronously in a separate thread).
    void startRendering( const Scalar dt );

    /// Blocks until rendering is finished.
    void waitForRendering();

    //
    // Misc functions
    //

    /// Emits signals corresponding to picking requests.
    void processPicking();

    /// Moves the camera so that the whole scene is visible.
    void fitCameraToScene( const Core::Aabb& sceneAabb );

    /// Returns the names of the different registred renderers.
    std::vector<std::string> getRenderersName() const;

    /// Write the current frame as an image. Supports either BMP or PNG file names.
    void grabFrame( const std::string& filename );

    /// Activates printing stat info to the error output after each glbinding call.
    void enableDebug();

  signals:
    /// Emitted when GL context is ready. We except call to addRenderer here
    void glInitialized();

    /// Emitted when the rendered is correctly initialized
    void rendererReady();

    /// Emitted when the result of a left click picking is known (for gizmo manip)
    void leftClickPicking( int id );

    /// Emitted when the resut of a right click picking is known (for selection)
    void rightClickPicking( const Ra::Engine::Renderer::PickingResult& result );

    /// Emitted when the corresponding key is released (see keyReleaseEvent)
    void toggleBrushPicking( bool on );

  public slots:
    /// Tell the renderer to reload all shaders.
    void reloadShaders();

    /// Set the final display texture.
    void displayTexture( const QString& tex );

    /// Set the renderer.
    bool changeRenderer( int index );

    /// Toggle the post-process effetcs.
    void enablePostProcess( int enabled );

    /// Toggle the debug drawing.
    void enableDebugDraw( int enabled );

    /** Add a renderer and return its index. Need to be called when catching
     * \param e : unique_ptr to your own renderer
     * \return index of the newly added renderer
     * \code
     * int rendererId = addRenderer(new MyRenderer(width(), height()));
     * changeRenderer(rendererId);
     * getRenderer()->initialize();
     * auto light = Ra::Core::make_shared<Engine::DirectionalLight>();
     * getRenderer()->addLight( light );
     * m_camera->attachLight( light );
     * \endcode
     */
    int addRenderer( std::shared_ptr<Engine::Renderer> e );

  private slots:
    /// \name Renderer slots
    /// These slots are connected to the base class signals to properly handle
    /// concurrent access to the renderer.
    ///@{
    void onAboutToCompose();
    void onAboutToResize();
    void onFrameSwapped();
    void onResized();
    ///@}

  protected:
    /// Initialize renderer internal state + configure lights.
    void intializeRenderer( Engine::Renderer* renderer );

    //
    // OpenGL primitives
    // Not herited, defined here in the same way QOpenGLWidget defines them.
    //

    /// Initialize openGL. Called on by the first "show" call to the main window.
    /// \warning This function is NOT reentrant, and may behave incorrectly
    /// if called at the same time than #intializeRenderer.
    virtual void initializeGL();

    /// Resize the view port and the camera. Called by the resize event.
    virtual void resizeGL( int width, int height );

    /// \name Qt input events.
    /// \note We intercept the mouse events in this widget to get the
    /// coordinates of the mouse in screen space.
    ///@{
    void resizeEvent( QResizeEvent* event ) override;

    void keyPressEvent( QKeyEvent* event ) override;

    void keyReleaseEvent( QKeyEvent* event ) override;

    void mousePressEvent( QMouseEvent* event ) override;

    void mouseReleaseEvent( QMouseEvent* event ) override;

    void mouseMoveEvent( QMouseEvent* event ) override;

    void wheelEvent( QWheelEvent* event ) override;

    void showEvent( QShowEvent* ev ) override;

    void exposeEvent( QExposeEvent* ev ) override;
    ///@}

    /// Return the Picking mode.
    Engine::Renderer::PickingMode getPickingMode() const;

  protected:
    /// OpenglContext used with this widget.
    std::unique_ptr<QOpenGLContext> m_context;

    /// The list of available renderers.
    std::vector<std::shared_ptr<Engine::Renderer>> m_renderers;

    /// The current Renderer.
    Engine::Renderer* m_currentRenderer;

    /// The PickingManager.
    PickingManager* m_pickingManager;

    /// Whether picking in the circular brush is active.
    bool m_isBrushPickingEnabled;

    /// The radius of the circular brush.
    float m_brushRadius;

    /// The Camera.
    std::unique_ptr<CameraInterface> m_camera;

    /// The GizmoManager.
    GizmoManager* m_gizmoManager;

    /// Thread in which rendering is done.
    // We have to use a QThread for MT rendering.
    // TODO: are we really using this ? Remove if we do not plan to do multi thread rendering.
    [[deprecated]] QThread* m_renderThread = nullptr;

    /// GL initialization status.
    std::atomic_bool m_glInitStatus;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
