#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <Core/CoreMacros.hpp>
#if defined (OS_WINDOWS)
#include <GL/glew.h>
#endif

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace Ra { namespace Core   { struct KeyEvent;    } }
namespace Ra { namespace Core   { struct MouseEvent;  } }
namespace Ra { namespace Engine { class RadiumEngine; } }
namespace Ra { namespace Engine { class Renderer;     } }
namespace Ra { namespace Engine { class Camera;       } }

namespace Ra { namespace Gui {

// FIXME (Charly) : Which way do we want to be able to change renderers ?
//					Can it be done during runtime ? Must it be at startup ? ...
//					For now, default ForwardRenderer is used.
class Viewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    /// CONSTRUCTOR
    explicit Viewer(QWidget* parent = nullptr);

    /// DESTRUCTOR
    ~Viewer();

    void quit();
    bool loadFile(const QString& path);

    Engine::RadiumEngine* getEngine() const { return m_engine; }

signals:
    void entitiesUpdated();

protected:
    /// OPENGL
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int width, int height) override;

    /// INTERACTION
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    void mouseEventQtToRadium(QMouseEvent* qtEvent, Core::MouseEvent* raEvent);
    void keyEventQtToRadium(QKeyEvent* qtEvent, Core::KeyEvent* raEvent);

private:
    Engine::RadiumEngine* m_engine;
    Engine::Renderer* m_renderer;

    Engine::Camera* m_camera;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
