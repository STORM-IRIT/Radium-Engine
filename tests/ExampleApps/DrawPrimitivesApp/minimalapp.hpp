#include <QApplication>
#include <QTimer>

#include <Core/Tasks/TaskQueue.hpp>
#include <GuiBase/Viewer/Viewer.hpp>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

/// Our minimal application uses QTimer to be called at a regular frame rate.
class MinimalApp : public QApplication
{
    Q_OBJECT

  public:
    /** IMPORTANT : the argc parameter must be a reference for calling the QApplication constructor
        @see http://doc.qt.io/qt-5/qapplication.html#QApplication
    */
    MinimalApp( int& argc, char** argv );

    ~MinimalApp();

  public slots:

    /// This function is the basic "game loop" iteration of the engine.
    /// It starts the rendering then advance all systems by one frame.
    void frame();
    void onGLInitialized();

  public:
    // Our instance of the engine
    std::unique_ptr<Ra::Engine::RadiumEngine> m_engine;

    // Task queue
    std::unique_ptr<Ra::Core::TaskQueue> m_task_queue;

    // Pointer to Qt/OpenGL Viewer widget.
    std::unique_ptr<Ra::Gui::Viewer> m_viewer;

    // Timer to wake us up at every frame start.
    QTimer* m_frame_timer;

    // Our framerate
    uint m_target_fps;

}; // end class
