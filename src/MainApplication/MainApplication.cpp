#include <MainApplication/MainApplication.hpp>

#include <Core/CoreMacros.hpp>

#include <iostream>
#include <QTimer>

#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Math/Vector.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <MainApplication/Gui/MainWindow.hpp>

namespace Ra
{
    MainApplication::MainApplication(int argc, char** argv)
        : QApplication(argc, argv)
        , m_mainWindow(nullptr)
        , m_engine(nullptr)
        , m_taskQueue(nullptr)
        , m_viewer(nullptr)
        , m_frameTimer(new QTimer(this))
    {
        // Boilerplate print.

        std::cerr << "*** Radium Engine Main App  ***" << std::endl;
#if defined (CORE_DEBUG)
        std::cerr << "(Debug Build) -- ";
#else
        std::cerr<<"(Release Build) -- ";
#endif

#if defined (ARCH_X86)
        std::cerr<<" 32 bits x86";
#elif defined (ARCH_X64)
        std::cerr << " 64 bits x64";
#endif
        std::cerr << std::endl;

        std::cerr << "Floating point format : ";
#if defined(CORE_USE_DOUBLE)
        std::cerr<<"double precision"<<std::endl;
#else
        std::cerr << "single precision" << std::endl;
#endif

        // Handle command line arguments.
        // TODO ( e.g fps limit)

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
        // initialized the OpenGL context..
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

        m_frameTime = QTime::currentTime();

        connect(m_frameTimer, SIGNAL(timeout()), this, SLOT(radiumFrame()));
        m_frameTimer->start(1000 / 60);
    }

	void MainApplication::createConnections()
	{
	}

    void MainApplication::loadFile(QString path)
    {
        std::string pathStr = path.toLocal8Bit().data();
        bool res = m_engine->loadFile(pathStr);

		if (res)
		{
			auto drawables = m_engine->getDrawableManager()->getDrawables();

			Core::Aabb sceneBBox;
			sceneBBox.setEmpty();

			for (const auto& drawable : drawables)
			{
				sceneBBox.extend(drawable->getBoundingBoxInWorld());
			}

			emit(sceneChanged(sceneBBox));
		}
    }

    void MainApplication::viewerReady(Gui::Viewer* viewer)
    {
        m_viewer = viewer;
        CORE_ASSERT( m_viewer->parent()->parent() == m_mainWindow.get(), "Viewer is not setup");
    }

    void MainApplication::radiumFrame()
    {
        QTime currentTime = QTime::currentTime();

        long elapsed = m_frameTime.msecsTo(currentTime);
        m_frameTime = currentTime;
        const Scalar dt = Scalar(elapsed)/ 1000.f;

        m_viewer->startRendering(dt);

        // Gather user input and dispatch it.
        auto keyEvents = m_mainWindow->getKeyEvents();
        auto mouseEvents = m_mainWindow->getMouseEvents();

        m_mainWindow->flushEvents();

        m_engine->getTasks(m_taskQueue.get(), dt);

        // Run one frame of tasks
        m_taskQueue->processTaskQueue();
        
        m_taskQueue->flushTaskQueue();


        // Block until frame is fully rendered.
        m_viewer->waitForRendering();
        m_viewer->update();

    }

    MainApplication::~MainApplication()
    {
        fprintf(stderr, "About to quit... Cleaning RadiumEngine memory.\n");
        emit stopping();
        m_engine->cleanup();
    }

}
