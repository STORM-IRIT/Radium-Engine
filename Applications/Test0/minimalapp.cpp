#include <minimalapp.hpp>

MinimalApp::MinimalApp(int &argc, char** argv)
        : QApplication(argc, argv), _engine(nullptr),
          _task_queue(nullptr),
          _viewer(nullptr),
          _frame_timer(nullptr),
          _target_fps(60)
{
    LOG(logDEBUG) << "Initialize engine.";
    _engine.reset(Ra::Engine::RadiumEngine::createInstance());
    _engine->initialize();
    LOG(logDEBUG) << "Initialize taskqueue.";
    _task_queue.reset(new Ra::Core::TaskQueue(std::thread::hardware_concurrency() - 1));
    LOG(logDEBUG) << "Initialize viewer.";
    _viewer.reset(new Ra::Gui::Viewer);

    CORE_ASSERT( _viewer != nullptr, "GUI was not initialized" );

    LOG(logDEBUG) << "Initialize timer.";
    _frame_timer = new QTimer(this);
    _frame_timer->setInterval(1000 / _target_fps);
    connect(_frame_timer, &QTimer::timeout, this, &MinimalApp::frame);
}

MinimalApp::~MinimalApp()
{
    _engine->cleanup();
}



void MinimalApp::frame()
{
    // We use a fixed time step, but it is also possible
    // to check the time from last frame.
    const Scalar dt = 1.f / Scalar(_target_fps);

    // Starts the renderer
    _viewer->startRendering(dt);

    // Collect and run tasks
    _engine->getTasks(_task_queue.get(), dt);
    _task_queue->startTasks();
    _task_queue->waitForTasks();
    _task_queue->flushTaskQueue();

    // Finish the frame
    _viewer->waitForRendering();
    //_viewer.update();
    _viewer->repaint();
    _engine->endFrameSync();
}
