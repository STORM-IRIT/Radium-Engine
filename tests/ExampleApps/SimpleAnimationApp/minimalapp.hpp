#include <QApplication>
#include <QTimer>

#include <GuiBase/TimerData/FrameTimerData.hpp>
#include <GuiBase/Viewer/Viewer.hpp>

#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

/* This file contains a minimal radium/qt application which shows the
 * "Simple Animation" demo, which uses KeyFramedValues to spin a cube with
 * time-varying colors. */

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

    // Timer to wake us up at every frame start.
    QTimer* m_close_timer;

    // Our framerate
    uint m_target_fps;

}; // end class
