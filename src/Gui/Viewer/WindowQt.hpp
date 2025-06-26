// This file si picked and slightly adapted from
// globjects/source/examples/qtexample/WindowQt.h
// so that update in globjects is easy
// Radium Viewer derive this class

#pragma once
#include <Gui/RaGui.hpp>

#include <QScopedPointer>
#include <QWindow>

#include <glbinding/ProcAddress.h>

#include <atomic>
#include <memory>

class QSurfaceFormat;
class QOpenGLContext;
class QEvent;
class QResizeEvent;
class QScreen;

namespace Ra {
namespace Gui {

/*!
 * \brief Base class for OpenGL widgets, compatble with Qt and globjects/glbindings.
 * \note This class has been extracted from globjects example
 * https://github.com/cginternals/globjects/blob/master/source/examples/qtexample/WindowQt.h
 */
class RA_GUI_API WindowQt : public QWindow
{
    Q_OBJECT
  private:
    class ScopedGLContext;

  public:
    explicit WindowQt( QScreen* screen );
    virtual ~WindowQt();

    void resizeEvent( QResizeEvent* event ) override;
    void showEvent( QShowEvent* event ) override;
    void exposeEvent( QExposeEvent* event ) override;
    // bool event( QEvent* event ) override;

    virtual void enterEvent( QEvent* event );
    virtual void leaveEvent( QEvent* event );

    /**
     * Make this->context() the current context.
     * This function is made reentrant thanks to an internal counter that count how many time the
     * context has been asked to be activated.
     * If the context have already been made current with a previous call to makeCurrent, this
     * function increase an internal counter by one so that doneCurrent do not release the context.
     * \see activateScopedContext() as an alternative
     */
    void makeCurrent();

    /**
     * Release this->context().
     * This results in no context being current in the current thread.
     * In case a context has been made current multiple times, this function just deacrease the
     * internal counter by one. \see makeCurrent()
     * \see activateScopedContext() as an alternative
     */
    void doneCurrent();

    /**
     * Make this->context() the current context for current block.
     * The context is released when the return variable is destroyed. The "context object" uses
     * this->makeCurrent() and this->doneCurrent(), hence it's compatible with enclosed scopes, and
     * only the first created context object dtor eventually release OpenGL context.
     *
     * Example
     *
     * \code{.cpp} if(test){
     *     auto context = viewer->activateScopedContext();
     *     // here OpenGL context is bound
     *     Texture tex;
     *     // [...] do tex setup
     *     tex->initializeGL();
     *     // [...] use tex ...
     * } // block exit will first call tex dtor (with active context) then context dtor.
     * \endcode
     */
    inline ScopedGLContext activateScopedContext();

    /// \see https://doc.qt.io/qt-5/qglwidget.html#context
    QOpenGLContext* context();

    // note when updating from globjets
    // updateGL done by base app rendering loop

    bool isOpenGlInitialized() const { return m_glInitialized.load(); }

  signals:
    /// Emitted when physical device changes. Useful for tracking devicePixelRatio() change.
    void dpiChanged();

  public slots:
    /// call deinitializeGL if needed, with context activated
    void cleanupGL();
    void screenChanged();

  private slots:
    void physicalDpiChanged( qreal dpi );

  protected:
    // OpenglContext used with this widget
    std::unique_ptr<QOpenGLContext> m_context;

    bool m_updatePending;
    std::atomic_bool m_glInitialized;

    void initialize();
    void resizeInternal( QResizeEvent* event );

    //
    // OpenGL related methods
    // Not inherited, defined here in the same way QOpenGLWidget define them.
    //
    /// Initialize the openGL related part of the Window
    virtual bool initializeGL();
    /// DeInitialize the OpenGL
    virtual void deinitializeGL();
    /// Resize the OpenGL related part of the Window
    virtual void resizeGL( QResizeEvent* event );

    // note when updating from globjets
    // paintGL done by base app rendering loop

  protected:
    static glbinding::ProcAddress getProcAddress( const char* name );

  private:
    /// prevent direct call to QWindow::create, not adapted to our implementation.
    /// use QWidget::createWindowContainer( WindowQt_ptr ) instead.;
    using QWindow::create;

    static WindowQt* s_getProcAddressHelper;
    int m_contextActivationCount { 0 };

    QMetaObject::Connection m_screenObserver;
};

class WindowQt::ScopedGLContext
{
  public:
    explicit ScopedGLContext( WindowQt* window ) : m_window( window ) { window->makeCurrent(); }
    ~ScopedGLContext() { m_window->doneCurrent(); }
    ScopedGLContext( const ScopedGLContext& )            = delete;
    ScopedGLContext& operator=( ScopedGLContext const& ) = delete;

  private:
    WindowQt* m_window;
};

inline WindowQt::ScopedGLContext WindowQt::activateScopedContext() {
    return ScopedGLContext { this };
}

} // namespace Gui
} // namespace Ra
