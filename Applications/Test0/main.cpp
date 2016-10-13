#include <QTimer>
#include <QObject>
#include <QApplication>
#include <QCoreApplication>

#include <Core/Time/Timer.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/RadiumEngine.hpp>

#include <GuiBase/Viewer/Viewer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>


namespace {

    /*
     * FIXME This class is a complete draft and has the only purpose to find a
     * way to run without crash (which would be cool).
     *
     * It may compile, however, I actually have a very poor idea of how the
     * system was built, so I am likely to have forgotten many important
     * things.
     *
     * The best ressource I found to achieve it is MainApplication.cpp
     *
     */

    class UglyApp : public QApplication {
    public:
        UglyApp(int& argc, char** argv)
            : QApplication(argc, argv)
            , _engine     ( Ra::Engine::RadiumEngine::createInstance() )
            , _task_queue ( new Ra::Core::TaskQueue( std::thread::hardware_concurrency() -1 ) )
            , _frame_timer( this )
            , _target_fps ( 60 )
        {
            // What time is it ?
            _last_frame_start = Ra::Core::Timer::Clock::now();

            // Qt connections
            // connect( this, notifyClose, this, aboutToQuit );
        }

        ~UglyApp() {}

        // void notifyClose() { _is_about_to_quit = true; }

    public:
        std::unique_ptr<Ra::Engine::RadiumEngine> _engine;
        std::unique_ptr<Ra::Core::TaskQueue>      _task_queue;

        // Pointer to OpenGL Viewer for render call (belongs to MainWindow).
        Ra::Gui::Viewer _viewer;

        // Timer to wake us up at every frame start.
        QTimer _frame_timer;

        // Time since the last frame start.
        Ra::Core::Timer::TimePoint _last_frame_start;

        uint _target_fps;
        bool _is_about_to_quit;

    }; // end class UglyApp
}

int main ( int argc, char *argv[] )
{
    // A generic application
    // May be replaced by the old tabletapplication
    UglyApp app ( argc, argv );

    app._engine->initialize();

    app._viewer.initRenderer();
    app._viewer.show();

    return app.exec();
}
