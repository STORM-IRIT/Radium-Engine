#include <minimalapp.hpp>

MinimalApp::MinimalApp(int &argc, char** argv)
        : QApplication(argc, argv), m_engine(nullptr),
          m_task_queue(nullptr),
          m_viewer(nullptr),
          m_frame_timer(nullptr),
          m_target_fps(60)
{
    LOG(logDEBUG) << "Initialize engine.";
    m_engine.reset(Ra::Engine::RadiumEngine::createInstance());
    m_engine->initialize();
    LOG(logDEBUG) << "Initialize taskqueue.";
    m_task_queue.reset(new Ra::Core::TaskQueue(std::thread::hardware_concurrency() - 1));
    LOG(logDEBUG) << "Initialize viewer.";
    m_viewer.reset(new Ra::Gui::Viewer);

    CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );

    LOG(logDEBUG) << "Initialize timer.";
    m_frame_timer = new QTimer(this);
    m_frame_timer->setInterval(1000 / m_target_fps);
    connect(m_frame_timer, &QTimer::timeout, this, &MinimalApp::frame);
}

MinimalApp::~MinimalApp()
{
    m_engine->cleanup();
}



void MinimalApp::frame()
{
    // We use a fixed time step, but it is also possible
    // to check the time from last frame.
    const Scalar dt = 1.f / Scalar(m_target_fps);

    // Starts the renderer
    m_viewer->startRendering(dt);

    // Collect and run tasks
    m_engine->getTasks(m_task_queue.get(), dt);
    m_task_queue->startTasks();
    m_task_queue->waitForTasks();
    m_task_queue->flushTaskQueue();

    // Finish the frame
    m_viewer->waitForRendering();
    //_viewer.update();
    m_viewer->repaint();
    m_engine->endFrameSync();
}
