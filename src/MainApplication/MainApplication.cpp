#include <MainApplication/MainApplication.hpp>

#include <Core/CoreMacros.hpp>

#include <QTimer>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <MainApplication/Gui/MainWindow.hpp>

// Const parameters : TODO : make config / command line options
const uint FPS_MAX = 60;
const uint TIMER_AVERAGE = 100;

namespace Ra
{
    MainApplication::MainApplication(int argc, char** argv)
        : QApplication(argc, argv)
        , m_mainWindow(nullptr)
        , m_engine(nullptr)
        , m_taskQueue(nullptr)
        , m_viewer(nullptr)
        , m_frameTimer(new QTimer(this))
        , m_frameCounter(0)
        //, m_timerData(TIMER_AVERAGE)
    {
        // Boilerplate print.

        LOG(logINFO) << "*** Radium Engine Main App  ***";
        std::stringstream config;
#if defined (CORE_DEBUG)
        config << "(Debug Build) -- ";
#else
        config << "(Release Build) -- ";
#endif

#if defined (ARCH_X86)
        config <<" 32 bits x86";
#elif defined (ARCH_X64)
        config << " 64 bits x64";
#endif
        LOG(logINFO) << config.str();

        config.str(std::string());
        config << "Floating point format : ";
#if defined(CORE_USE_DOUBLE)
        config << "double precision" << std::endl;
#else
        config << "single precision" << std::endl;
#endif

        LOG(logINFO) << config.str();
        LOG(logINFO) << "(Log using default file)";

        // Handle command line arguments.
        // TODO ( e.g fps limit ) / Keep or not timer data .

        // Create default format for Qt.
        QSurfaceFormat format;
        format.setVersion(4, 4);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(0);
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setSamples(16);
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        QSurfaceFormat::setDefaultFormat(format);

        // Create main window.
        m_mainWindow.reset(new Gui::MainWindow);
        m_mainWindow->show();

        // Allow all events to be processed (thus the viewer should have
        // initialized the OpenGL context..)
        processEvents();

        m_viewer = m_mainWindow->getViewer();
        CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized");
        CORE_ASSERT( m_viewer->context()->isValid(), "OpenGL was not initialized" );

        // Create engine
        m_engine.reset(new Engine::RadiumEngine);
        m_engine->initialize();
        m_engine->setupScene();

        // Pass the engine to the renderer to complete the initialization process.
        m_viewer->initRenderer(m_engine.get());

        // Create task queue with N-1 threads (we keep one for rendering).
        m_taskQueue.reset(new Core::TaskQueue(std::thread::hardware_concurrency() - 1));

		createConnections();

        emit starting();

        m_lastFrameStart = Core::Timer::Clock::now();

        connect(m_frameTimer, SIGNAL(timeout()), this, SLOT(radiumFrame()));
        m_frameTimer->start(1000 / FPS_MAX);
    }

	void MainApplication::createConnections()
	{
	}

    void MainApplication::loadFile(QString path)
    {
        std::string pathStr = path.toLocal8Bit().data();
        bool res = m_engine->loadFile(pathStr);
        m_viewer->handleFileLoading(pathStr);
    }

    void MainApplication::radiumFrame()
    {
        FrameTimerData timerData;
        timerData.frameStart = Core::Timer::Clock::now();

        // ----------
        // 0. Compute time since last frame.
        const Scalar dt = Core::Timer::getIntervalSeconds( m_lastFrameStart, timerData.frameStart);
        m_lastFrameStart = timerData.frameStart;

        // ----------
        // 1. Kickoff rendering
        m_viewer->startRendering(dt);

        // ----------
        // 2. Gather user input and dispatch it.
        auto keyEvents = m_mainWindow->getKeyEvents();
        auto mouseEvents = m_mainWindow->getMouseEvents();
        m_mainWindow->flushEvents();

        timerData.tasksStart = Core::Timer::Clock::now();

        // ----------
        // 3. Run the engine task queue.
        m_engine->getTasks(m_taskQueue.get(), dt);

        // Run one frame of tasks
        m_taskQueue->processTaskQueue();
        timerData.taskData = m_taskQueue->getTimerData();
        m_taskQueue->flushTaskQueue();

        timerData.tasksEnd = Core::Timer::Clock::now();

        // ----------
        // 4. Wait until frame is fully rendered and display.
        m_viewer->waitForRendering();
        m_viewer->update();

        timerData.renderData = m_viewer->getRenderer()->getTimerData();

        // ----------
        // 5. Frame end.
        timerData.frameEnd = Core::Timer::Clock::now();
        timerData.numFrame = m_frameCounter;
        //m_timerData.addFrame(timerData);
        ++m_frameCounter;

        // Dump timer data if requested.
        //LOG_EVERY_N(TIMER_AVERAGE, INFO) << m_timerData;
//        if (TIMER_AVERAGE == 1)
//        {
//            FrameTimerData::printTimerData(timerData);
//        }
//        else if (TIMER_AVERAGE > 1 && m_frameCounter % TIMER_AVERAGE == 0)
//        {
//            FrameTimerData::printAverageTimerData(m_timerData);
//            m_timerData.clear();
//        }
    }

    MainApplication::~MainApplication()
    {
        LOG(logINFO) << "About to quit... Cleaning RadiumEngine memory";
        emit stopping();
        m_engine->cleanup();
    }

}
