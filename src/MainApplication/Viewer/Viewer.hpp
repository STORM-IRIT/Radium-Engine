#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <Core/CoreMacros.hpp>
#if defined (OS_WINDOWS)
#include <GL/glew.h>
#endif

#include <memory>
#include <thread>
#include <mutex>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <Core/Math/Vector.hpp>

namespace Ra { namespace Core   { struct KeyEvent;       } }
namespace Ra { namespace Core   { struct MouseEvent;     } }
namespace Ra { namespace Engine { class RadiumEngine;    } }
namespace Ra { namespace Engine { class Renderer;        } }

namespace Ra { namespace Gui    { class CameraInterface; } }

namespace Ra { namespace Gui {

// FIXME (Charly) : Which way do we want to be able to change renderers ?
//					Can it be done during runtime ? Must it be at startup ? ...
//					For now, default ForwardRenderer is used.



/// The Viewer is the main display class. It renders the OpenGL scene on screen
/// and will receive the raw user input (e.g. clicks and key presses) too.
class Viewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

private:
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

    void setRadiumEngine(Engine::RadiumEngine* engine);
    CameraInterface* getCamera() { return m_camera.get(); }

    void startRendering();

    void waitForRendering();
signals:
    void ready( Gui::Viewer* );
    void entitiesUpdated();

public slots:
    void reloadShaders();
	void sceneChanged(const Core::Aabb& bbox);

protected:
    /// OPENGL
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int width, int height) override;

    virtual void paintEvent(QPaintEvent* e) override {}
    /// INTERACTION
    // We intercept the mouse events in this widget to get the coordinates of the mouse
    // in screen space.
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

private:

    void runRenderThread();

	// FIXME(Charly): Find a better way to handle mouse events ?
	static Core::MouseEvent mouseEventQtToRadium(const QMouseEvent* qtEvent);

private:
    Engine::RadiumEngine* m_engine;
    Engine::Renderer* m_renderer;
    
    std::unique_ptr<CameraInterface> m_camera;
    std::thread m_renderThread;
    std::mutex m_cameraMutex;
    std::mutex m_rendererMutex;

    InteractionState m_interactionState;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
