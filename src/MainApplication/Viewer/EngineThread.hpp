#ifndef RADIUMENGINE_ENGINETHREAD_HPP
#define RADIUMENGINE_ENGINETHREAD_HPP

#include <QThread>
#include <QSurfaceFormat>

class QSurface;
class QOpenGLContext;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

namespace Ra
{

class RenderSystem;
class ViewerMT;

class EngineThread : public QThread
{
    Q_OBJECT

public:
    EngineThread(ViewerMT* viewer, QOpenGLContext* context,
                 QSurface* surface, QSurfaceFormat format);
    ~EngineThread();

    void initialize(uint width, uint height);
    void resize(uint width, uint height);
    virtual void run() override;

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event);


signals:
    void finished();

private:
    ViewerMT* m_guiViewer;
    RenderSystem* m_renderer;

    QOpenGLContext* m_glContext;
    QSurface* m_surface;
    QSurfaceFormat m_format;

    bool m_running;
};

}

#endif // RADIUMENGINE_ENGINETHREAD_HPP
