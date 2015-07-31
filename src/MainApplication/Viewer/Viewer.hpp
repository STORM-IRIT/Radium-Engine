#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <Core/CoreMacros.hpp>
#if defined (OS_WINDOWS)
#include <GL/glew.h>
#endif

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QThread>

#include <Core/Math/Vector.hpp>
#include <Engine/RadiumEngine.hpp>

// Uncomment this to deactivate multi-threaded rendering.
// In that case the call to startRendering() is synchronous
// and waitForRendering() does nothing.
// #define FORCE_RENDERING_ON_MAIN_THREAD

// Forward declarations
namespace Ra { namespace Core   { struct KeyEvent;       } }
namespace Ra { namespace Core   { struct MouseEvent;     } }
namespace Ra { namespace Engine { class RadiumEngine;    } }
namespace Ra { namespace Engine { class Renderer;        } }
namespace Ra { namespace Gui    { class CameraInterface; } }

namespace Ra { namespace Gui {

// FIXME (Charly) : Which way do we want to be able to change renderers ?
//					Can it be done during runtime ? Must it be at startup ? ...
//					For now, default ForwardRenderer is used.


/// The Viewer is the main display class. It's the central screen QWidget.
/// Its acts as a bridge between the interface, the engine and the renderer
/// Among its responsibilities are :
/// * Owning the renderer and camera, and managing their lifetime.
/// * setting up the renderer and camera by keeping it informed of interfaces changes
//  (e.g. resize).
/// * catching user interaction (mouse clicks) at the lowest level and forward it to
/// the camera and the rest of the application
/// * Expose the asynchronous rendering interface
class Viewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    /// State representation of the mode in which the UI is to be processed
    enum InteractionState
    {
        NONE,
        CAMERA,
        MANIPULATION
    };

public:
    /// CONSTRUCTOR
    explicit Viewer(QWidget* parent = nullptr);

    /// DESTRUCTOR
    ~Viewer();

    void initRenderer(Engine::RadiumEngine* engine);

    /// Access to camera interface.
    CameraInterface* getCamera() { return m_camera.get(); }

    /// Read-only access to renderer
    const Engine::Renderer* getRenderer() { return m_renderer.get(); }


    /// Start asynchronous rendering in a separate thread.
    void startRendering(const Scalar dt);

    /// Blocks until rendering is finished.
    void waitForRendering();

public slots:
    /// Tell the renderer to reload all shaders.
    void reloadShaders();

private slots:
    // These slots are connected to the base class signals to properly handle
    // concurrent access to the renderer.
    void onAboutToCompose();
    void onAboutToResize();
    void onFrameSwapped();
    void onResized();

private:
    /// QOpenGlWidget primitives
    // Initialize openGL. Called on by the first "show" call to the main window.
    virtual void initializeGL() override;

    // Resize the view port and the camera. Called by the resize event.
    virtual void resizeGL(int width, int height) override;

    // Paint event is set to a no-op to prevent synchronous rendering.
    // We don't implement paintGL as well.
    virtual void paintEvent(QPaintEvent* e) override {}

    /// INTERACTION
    // We intercept the mouse events in this widget to get the coordinates of the mouse
    // in screen space.
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

private:
    /// Owning pointer to the renderer.
    std::unique_ptr<Engine::Renderer> m_renderer;

    /// Owning pointer to the camera
    std::unique_ptr<CameraInterface> m_camera;

    /// Keeps the state on which we should interpret user input.
    InteractionState m_interactionState;

    /// Thread in which rendering is done.
    QThread* m_renderThread; // We have to use a QThread for MT rendering
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
