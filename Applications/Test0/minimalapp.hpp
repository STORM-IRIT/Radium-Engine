#include <QApplication>
#include <QTimer>

#include <GuiBase/Viewer/Viewer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>

#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

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
    MinimalApp(int &argc, char** argv);

    ~MinimalApp();


public slots:

    /// This function is the basic "game loop" iteration of the engine.
    /// It starts the rendering then advance all systems by one frame.
    void frame();

public:
    // Our instance of the engine
    std::unique_ptr<Ra::Engine::RadiumEngine> _engine;

    // Task queue
    std::unique_ptr<Ra::Core::TaskQueue> _task_queue;

    // Pointer to Qt/OpenGL Viewer widget.
    std::unique_ptr<Ra::Gui::Viewer>  _viewer;

    // Timer to wake us up at every frame start.
    QTimer* _frame_timer;

    // Our framerate
    uint _target_fps;

}; // end class

