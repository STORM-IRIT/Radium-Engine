#include <MainApplication/MainApplication.hpp>

#include <Core/CoreMacros.hpp>

#include <QTimer>
#include <QDir>
#include <QPluginLoader>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/String/StringUtils.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <MainApplication/Gui/MainWindow.hpp>
#include <MainApplication/Version.hpp>

#include <MainApplication/PluginBase/RadiumPluginInterface.hpp>

// Const parameters : TODO : make config / command line options


namespace Ra
{
    MainApplication::MainApplication( int argc, char** argv )
        : QApplication( argc, argv )
        , m_mainWindow( nullptr )
        , m_engine( nullptr )
        , m_taskQueue( nullptr )
        , m_viewer( nullptr )
        , m_frameTimer( new QTimer( this ) )
        , m_frameCounter( 0 )
        , m_isAboutToQuit( false )
        //, m_timerData(TIMER_AVERAGE)
    {

        std::string pluginsPath;
        if ( argc > 1 )
        {
            for ( int i = 1; i < argc; ++i )
            {
                std::string arg( argv[i] );

                if ( arg == "--pluginsPath" )
                {
                    pluginsPath = std::string( argv[i+1] );
                    continue;
                }
            }
        }
        if ( pluginsPath.empty() )
        {
            pluginsPath = "../Plugins/bin";
        }

        // Boilerplate print.
        LOG( logINFO ) << "*** Radium Engine Main App  ***";
        std::stringstream config;
#if defined (CORE_DEBUG)
        config << "(Debug Build) -- ";
#else
        config << "(Release Build) -- ";
#endif

#if defined (ARCH_X86)
        config << " 32 bits x86";
#elif defined (ARCH_X64)
        config << " 64 bits x64";
#endif
        LOG( logINFO ) << config.str();

        config.str( std::string() );
        config << "Floating point format : ";
#if defined(CORE_USE_DOUBLE)
        config << "double precision";
#else
        config << "single precision" ;
#endif

        LOG( logINFO ) << config.str();

        config.str( std::string() );
        config<<"build: "<<Version::compiler<<" - "<<Version::compileDate<<" "<<Version::compileTime;


        LOG( logINFO ) << config.str();

        LOG(logINFO) << "Qt Version: " << qVersion();

        // Handle command line arguments.
        // TODO ( e.g fps limit ) / Keep or not timer data .

        // Create default format for Qt.
        QSurfaceFormat format;
        format.setVersion( 4, 4 );
        format.setProfile( QSurfaceFormat::CoreProfile );
        format.setDepthBufferSize( 24 );
        format.setStencilBufferSize( 8 );
        format.setSamples( 16 );
        format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
        format.setSwapInterval( 0 );
        QSurfaceFormat::setDefaultFormat( format );

        // Create engine
        m_engine.reset(Engine::RadiumEngine::createInstance());
        m_engine->initialize();

        // Create main window.
        m_mainWindow.reset( new Gui::MainWindow );
        m_mainWindow->show();

        // Allow all events to be processed (thus the viewer should have
        // initialized the OpenGL context..)
        processEvents();

        // Load plugins
        if ( !loadPlugins( pluginsPath ) )
        {
            LOG( logERROR ) << "An error occured while trying to load plugins.";
        }

        m_viewer = m_mainWindow->getViewer();
        CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );
        CORE_ASSERT( m_viewer->context()->isValid(), "OpenGL was not initialized" );

        // Pass the engine to the renderer to complete the initialization process.
        m_viewer->initRenderer();

        // Create task queue with N-1 threads (we keep one for rendering).
        m_taskQueue.reset( new Core::TaskQueue( std::thread::hardware_concurrency() - 1 ) );

        createConnections();

        setupScene();
        emit starting();

        m_lastFrameStart = Core::Timer::Clock::now();
    }

    void MainApplication::createConnections()
    {
        connect( m_mainWindow.get(), &Gui::MainWindow::closed , this, &MainApplication::appNeedsToQuit );
    }

    void MainApplication::setupScene()
    {
        /*
        Engine::SystemEntity::uiCmp()->addRenderObject(
            Engine::DrawPrimitives::Grid(
                        Engine::SystemEntity::uiCmp(),
                        Core::Vector3::Zero(), Core::Vector3::UnitX(), Core::Vector3::UnitZ(),
                        Core::Colors::Grey(0.6f)));
                        */
    }

    void MainApplication::loadFile( QString path )
    {
        std::string pathStr = path.toLocal8Bit().data();
        bool res = m_engine->loadFile( pathStr );
        CORE_UNUSED( res );
        m_viewer->handleFileLoading( pathStr );

        // Compute new scene aabb
        Core::Aabb aabb;

        std::vector<std::shared_ptr<Engine::RenderObject>> ros;
        m_engine->getRenderObjectManager()->getRenderObjects( ros );

        for ( auto ro : ros )
        {
            auto mesh = ro->getMesh();
            auto pos = mesh->getGeometry().m_vertices;

            for ( auto& p : pos )
            {
                p = ro->getLocalTransform() * p;
            }

            Ra::Core::Vector3 bmin = pos.getMap().rowwise().minCoeff().head<3>();
            Ra::Core::Vector3 bmax = pos.getMap().rowwise().maxCoeff().head<3>();

            aabb.extend( bmin );
            aabb.extend( bmax );
        }

        m_viewer->fitCameraToScene( aabb );
    }

    void MainApplication::framesCountForStatsChanged( int count )
    {
        m_frameCountBeforeUpdate = count;
    }

    void MainApplication::radiumFrame()
    {
        FrameTimerData timerData;
        timerData.frameStart = Core::Timer::Clock::now();

        // ----------
        // 0. Compute time since last frame.
        const Scalar dt = Core::Timer::getIntervalSeconds( m_lastFrameStart, timerData.frameStart );
        m_lastFrameStart = timerData.frameStart;

        // ----------
        // 1. Gather user input and dispatch it.
        auto keyEvents = m_mainWindow->getKeyEvents();
        auto mouseEvents = m_mainWindow->getMouseEvents();

        // Get picking results from last frame and forward it to the selection.
        m_viewer->processPicking();

        m_mainWindow->flushEvents();

        // ----------
        // 2. Kickoff rendering
        m_viewer->startRendering( dt );

        timerData.tasksStart = Core::Timer::Clock::now();

        // ----------
        // 3. Run the engine task queue.
        m_engine->getTasks( m_taskQueue.get(), dt );

        // Run one frame of tasks
        m_taskQueue->startTasks();
        m_taskQueue->waitForTasks();
        timerData.taskData = m_taskQueue->getTimerData();
        m_taskQueue->flushTaskQueue();

        timerData.tasksEnd = Core::Timer::Clock::now();

        // ----------
        // 4. Wait until frame is fully rendered and display.
        m_viewer->waitForRendering();
        m_viewer->update();

        timerData.renderData = m_viewer->getRenderer()->getTimerData();

        // ----------
        // 5. Synchronize whatever needs synchronisation
        m_engine->endFrameSync();

        // ----------
        // 6. Frame end.
        timerData.frameEnd = Core::Timer::Clock::now();
        timerData.numFrame = m_frameCounter;

        m_timerData.push_back( timerData );
        ++m_frameCounter;

        if ( m_frameCounter % m_frameCountBeforeUpdate == 0 )
        {
            emit( updateFrameStats( m_timerData ) );
            m_timerData.clear();
        }
        emit endFrame();
    }

    void MainApplication::appNeedsToQuit()
    {
        LOG( logDEBUG ) << "About to quit.";
        m_isAboutToQuit = true;
    }

    MainApplication::~MainApplication()
    {
        LOG( logINFO ) << "About to quit... Cleaning RadiumEngine memory";
        emit stopping();
        m_mainWindow->cleanup();
        m_engine->cleanup();
    }

    bool MainApplication::loadPlugins( const std::string& pluginsPath )
    {
        LOG( logINFO )<<" *** Loading Plugins ***";
        QDir pluginsDir( qApp->applicationDirPath() );
        pluginsDir.cd( pluginsPath.c_str() );

        bool res = true;

        foreach (QString filename, pluginsDir.entryList( QDir::Files ) )
        {
            std::string ext = Core::StringUtils::getFileExt( filename.toStdString() );
#if defined( OS_WINDOWS )
            std::string sysDllExt = "dll";
#elif defined( OS_LINUX )
            std::string sysDllExt = "so";
#elif defined( OS_MACOS )
            std::string sysDllExt = "dylib";
#else
            static_assert( false, "System configuration not handled" );
#endif
            if ( ext == sysDllExt )
            {
                QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( filename ) );

                LOG( logINFO ) << "Found plugin " << filename.toStdString();

                QObject* plugin = pluginLoader.instance();
                Plugins::RadiumPluginInterface* loadedPlugin;

                if ( plugin )
                {
                    loadedPlugin = qobject_cast<Plugins::RadiumPluginInterface*>( plugin );
                    if ( loadedPlugin )
                    {
                        loadedPlugin->registerPlugin( m_engine.get() );
                        m_mainWindow->updateUi( loadedPlugin );
                    }
                    else
                    {
                        LOG( logERROR ) << "Something went wrong while trying to cast plugin"
                                        << filename.toStdString();
                        res = false;
                    }
                }
                else
                {
                    LOG( logERROR ) << "Something went wrong while trying to load plugin "
                                    << filename.toStdString() << " : "
                                    << pluginLoader.errorString().toStdString();
                    res = false;
                }
            }
        }

        return res;
    }
}
