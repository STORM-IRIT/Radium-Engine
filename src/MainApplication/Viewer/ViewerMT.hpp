#ifndef RADIUMENGINE_VIEWERMT_HPP
#define RADIUMENGINE_VIEWERMT_HPP

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <Engine/Renderer/RenderSystem.hpp>

namespace Ra
{

class EngineThread;

// FIXME (Charly) : Which way do we want to be able to change renderers ?
//					Can it be done during runtime ? Must it be at startup ? ...
//					For now, default ForwardRenderer is used.
class ViewerMT : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    friend class EngineThread;

public:
    /// CONSTRUCTOR
    explicit ViewerMT(QWidget* parent = nullptr);

    /// DESTRUCTOR
    ~ViewerMT();

public slots:
    void clearEngine();

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
//    virtual void closeEvent(QCloseEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override {}
//    virtual void resizeEvent(QResizeEvent* event) override;

private:


private:
    void mouseEventQtToRadium(QMouseEvent* qtEvent, MouseEvent* raEvent);
    void keyEventQtToRadium(QKeyEvent* qtEvent, KeyEvent* raEvent);

private:
//    std::shared_ptr<RenderSystem> m_renderer;
    EngineThread* m_engineThread;
};

} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
