#include <GuiBase/BaseApplication.hpp>

#include <GuiBase/MainWindowInterface.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Utils/Version.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/ItemModel/ItemEntry.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <PluginBase/RadiumPluginInterface.hpp>
#include <GuiBase/Utils/KeyMappingManager.hpp>

#ifdef IO_USE_TINYPLY
    #include <IO/TinyPlyLoader/TinyPlyFileLoader.hpp>
#endif
#ifdef IO_USE_ASSIMP
    #include <IO/AssimpLoader/AssimpFileLoader.hpp>
#endif

#include <QTimer>
#include <QDir>
#include <QPluginLoader>
#include <QCommandLineParser>
#include <QOpenGLContext>

#include <algorithm>


// Const parameters : TODO : make config / command line options


namespace Ra
{
namespace GuiBase
{
    BaseApplication::BaseApplication( int argc, char** argv, const WindowFactory &factory, QString applicationName, QString organizationName)
        : QApplication( argc, argv )
        , m_mainWindow( nullptr )
        , m_engine( nullptr )
        , m_taskQueue( nullptr )
        , m_viewer( nullptr )
        , m_frameTimer( new QTimer( this ) )
        , m_frameCounter( 0 )
        , m_numFrames( 0 )
        , m_maxThreads( RA_MAX_THREAD )
        , m_realFrameRate( false )
        , m_recordFrames( false )
        , m_recordTimings( false )
        , m_recordGraph( false )
        , m_isAboutToQuit( false )
    {
        // Set application and organization names in order to ensure uniform
        // QSettings configurations.
        // \see http://doc.qt.io/qt-5/qsettings.html#QSettings-4
        QCoreApplication::setOrganizationName(organizationName);
        QCoreApplication::setApplicationName(applicationName);

        m_targetFPS = 60; // Default
        std::string pluginsPath = "Plugins";

        QCommandLineParser parser;
        parser.setApplicationDescription("Radium Engine RPZ, TMTC");
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption fpsOpt(QStringList{"r", "framerate", "fps"}, "Control the application framerate, 0 to disable it (and run as fast as possible).", "number", "60");
        QCommandLineOption maxThreadsOpt(QStringList{"m", "maxthreads", "max-threads"}, "Control the maximum number of threads. 0 will set to the number of cores available", "number", "0");
        QCommandLineOption numFramesOpt(QStringList{"n", "numframes"}, "Run for a fixed number of frames.", "number", "0");
        QCommandLineOption pluginOpt(QStringList{"p", "plugins", "pluginsPath"}, "Set the path to the plugin dlls.", "folder", "Plugins");
        QCommandLineOption pluginLoadOpt(QStringList{"l", "load", "loadPlugin"}, "Only load plugin with the given name (filename without the extension). If this option is not used, all plugins in the plugins folder will be loaded. ", "name");
        QCommandLineOption pluginIgnoreOpt(QStringList{"i", "ignore", "ignorePlugin"}, "Ignore plugins with the given name. If the name appears within both load and ignore options, it will be ignored.", "name");
        QCommandLineOption fileOpt(QStringList{"f", "file", "scene"}, "Open a scene file at startup.", "file name", "foo.bar");

        parser.addOptions({fpsOpt, pluginOpt, pluginLoadOpt, pluginIgnoreOpt, fileOpt, maxThreadsOpt, numFramesOpt });
        parser.process(*this);

        if (parser.isSet(fpsOpt))       m_targetFPS = parser.value(fpsOpt).toUInt();
        if (parser.isSet(pluginOpt))    pluginsPath = parser.value(pluginOpt).toStdString();
        if (parser.isSet(numFramesOpt)) m_numFrames = parser.value(numFramesOpt).toUInt();
        if (parser.isSet(maxThreadsOpt)) m_maxThreads = parser.value(maxThreadsOpt).toUInt();


        std::time_t startTime = std::time(nullptr);
        std::tm* startTm = std::localtime(&startTime);
        Ra::Core::StringUtils::stringPrintf(m_exportFoldername, "%4u%02u%02u-%02u%02u",
                                            1900 + startTm->tm_year,
                                            startTm->tm_mon,
                                            startTm->tm_mday,
                                            startTm->tm_hour,
                                            startTm->tm_min);


        QDir().mkdir(m_exportFoldername.c_str());

        // Boilerplate print.
        LOG( logINFO ) << "*** Radium Engine Main App  ***";
        std::stringstream config;
#if defined (CORE_DEBUG)
        config << "Debug Build ";
#else
        config << "Release Build ";
#endif
#if defined (CORE_ENABLE_ASSERT)
        config<< "(with asserts) --";
#else
        config<<" --";
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
        config << "Texture support : ";
#if defined(RADIUM_WITH_TEXTURES)
        config << "enabled";
#else
        config << "disabled" ;
#endif

        LOG( logINFO ) << config.str();

        config.str( std::string() );
        config<<"core build: "<<Version::compiler<<" - "<<Version::compileDate<<" "<<Version::compileTime;
        LOG( logINFO ) << config.str();

        LOG( logINFO ) << "Git changeset: " << Version::gitChangeSet;

        LOG( logINFO ) << "Qt Version: " << qVersion();

        LOG( logINFO ) << "Max Thread: " << m_maxThreads;

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

        // Create the instance of the keymapping manager, before creating
        // Qt main windows, which may throw events on Microsoft Windows
        Gui::KeyMappingManager::createInstance();

        // Create engine
        m_engine.reset(Engine::RadiumEngine::createInstance());
        m_engine->initialize();
        addBasicShaders();
        // Create main window.
        m_mainWindow.reset( factory.createMainWindow() );
        m_mainWindow->show();

        m_viewer = m_mainWindow->getViewer();
        CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );
        CORE_ASSERT( m_viewer->getContext() != nullptr, "OpenGL context was not created" );
        CORE_ASSERT( m_viewer->getContext()->isValid(), "OpenGL was not initialized" );

        // Connect the signals and allow all pending events to be processed
        // (thus the viewer should have initialized the OpenGL context..)
        createConnections();
        processEvents();

        Ra::Engine::RadiumEngine::getInstance()->getEntityManager()->createEntity("Test");
        // Load plugins
        if ( !loadPlugins( pluginsPath, parser.values(pluginLoadOpt), parser.values(pluginIgnoreOpt) ) )
        {
            LOG( logERROR ) << "An error occurred while trying to load plugins.";
        }
        // Make builtin loaders the fallback if no plugins can load some file format
#ifdef IO_USE_TINYPLY
        // Register before AssimpFileLoader, in order to ease override of such
        // custom loader (first loader able to load is taking the file)
        m_engine->registerFileLoader( std::shared_ptr<Asset::FileLoaderInterface>(new IO::TinyPlyFileLoader()) );
#endif
#ifdef IO_USE_ASSIMP
        m_engine->registerFileLoader( std::shared_ptr<Asset::FileLoaderInterface>(new IO::AssimpFileLoader()) );
#endif

        // Create task queue with N-1 threads (we keep one for rendering),
        // unless monothread CPU
        uint numThreads =  std::max( m_maxThreads == 0 ? RA_MAX_THREAD : std::min(m_maxThreads, RA_MAX_THREAD), 1u);
        m_taskQueue.reset( new Core::TaskQueue(numThreads) );

        setupScene();
        emit starting();

        // A file has been required, load it.
        if (parser.isSet(fileOpt))
        {
            loadFile(parser.value(fileOpt));
        }

        m_lastFrameStart = Core::Timer::Clock::now();
    }

    void BaseApplication::createConnections()
    {
        connect( m_mainWindow.get(), &MainWindowInterface::closed , this, &BaseApplication::appNeedsToQuit );
        connect( m_viewer, &Gui::Viewer::glInitialized, this, &BaseApplication::initializeOpenGlPlugins );
    }

    void BaseApplication::setupScene()
    {

        using namespace Engine::DrawPrimitives;

        auto grid = Primitive(Engine::SystemEntity::uiCmp(),
                              Grid( Core::Vector3::Zero(), Core::Vector3::UnitX(),
                                    Core::Vector3::UnitZ(), Core::Colors::Grey(0.6f) ));
        grid->setPickable( false );
        Engine::SystemEntity::uiCmp()->addRenderObject(grid);

        auto frame = Primitive(Engine::SystemEntity::uiCmp(), Frame(Ra::Core::Transform::Identity(), 0.05f));
        frame->setPickable( false );
        Engine::SystemEntity::uiCmp()->addRenderObject(frame);

        // FIXME (Florian): this should disappear
        auto em =  Ra::Engine::RadiumEngine::getInstance()->getEntityManager();
        Ra::Engine::Entity* e = em->entityExists("Test") ?
            Ra::Engine::RadiumEngine::getInstance()->getEntityManager()->getEntity("Test"):
            Ra::Engine::RadiumEngine::getInstance()->getEntityManager()->createEntity("Test");

        for (auto& c: e->getComponents())
        {
            c->initialize();
        }

    }

    void BaseApplication::loadFile( QString path )
    {
        std::string pathStr = path.toLocal8Bit().data();
        LOG(logINFO) << "Loading file " << pathStr << "...";
        bool res = m_engine->loadFile( pathStr );

        if ( !res )
        {
           LOG ( logERROR ) << "Aborting file loading !";

            return;
        }

        m_viewer->handleFileLoading( m_engine->getFileData() );

        m_engine->releaseFile();

        m_mainWindow->postLoadFile();

        emit loadComplete();
    }

    void BaseApplication::framesCountForStatsChanged( uint count )
    {
        m_frameCountBeforeUpdate = count;
    }

    void BaseApplication::addBasicShaders()
    {
        using namespace Ra::Engine;

        ShaderConfiguration bpConfig("BlinnPhong");
        bpConfig.addShader(ShaderType_VERTEX, "Shaders/BlinnPhong.vert.glsl");
        bpConfig.addShader(ShaderType_FRAGMENT, "Shaders/BlinnPhong.frag.glsl");
        ShaderConfigurationFactory::addConfiguration(bpConfig);

        ShaderConfiguration pConfig("Plain");
        pConfig.addShader(ShaderType_VERTEX, "Shaders/Plain.vert.glsl");
        pConfig.addShader(ShaderType_FRAGMENT, "Shaders/Plain.frag.glsl");
        ShaderConfigurationFactory::addConfiguration(pConfig);

        ShaderConfiguration lgConfig("LinesGeom");
        lgConfig.addShader(ShaderType_VERTEX, "Shaders/Lines.vert.glsl");
        lgConfig.addShader(ShaderType_FRAGMENT, "Shaders/Lines.frag.glsl");
        lgConfig.addShader(ShaderType_GEOMETRY, "Shaders/Lines.geom.glsl");
        ShaderConfigurationFactory::addConfiguration(lgConfig);

        ShaderConfiguration lagConfig("LinesAdjacencyGeom");
        lagConfig.addShader(ShaderType_VERTEX, "Shaders/Lines.vert.glsl");
        lagConfig.addShader(ShaderType_FRAGMENT, "Shaders/LinesAdjacency.frag.glsl");
        lagConfig.addShader(ShaderType_GEOMETRY, "Shaders/Lines.geom.glsl");
        ShaderConfigurationFactory::addConfiguration(lagConfig);

        ShaderConfiguration lConfig("Lines");
        lConfig.addShader(ShaderType_VERTEX, "Shaders/Lines.vert.glsl");
        lConfig.addShader(ShaderType_FRAGMENT, "Shaders/Lines.frag.glsl");
        ShaderConfigurationFactory::addConfiguration(lConfig);

        ShaderConfiguration gdConfig("GradientDisplay");
        lConfig.addShader(ShaderType_VERTEX, "Shaders/GradientDisplay.vert.glsl");
        lConfig.addShader(ShaderType_FRAGMENT, "Shaders/GradientDisplay.frag.glsl");
        ShaderConfigurationFactory::addConfiguration(gdConfig);
    }

    void BaseApplication::radiumFrame()
    {
        FrameTimerData timerData;
        timerData.frameStart = Core::Timer::Clock::now();

        // ----------
        // 0. Compute time since last frame.
        const Scalar dt = m_realFrameRate ?
                    Core::Timer::getIntervalSeconds( m_lastFrameStart, timerData.frameStart ) :
                    1.f / Scalar(m_targetFPS);
        m_lastFrameStart = timerData.frameStart;

        timerData.eventsStart = Core::Timer::Clock::now();
        processEvents();
        timerData.eventsEnd = Core::Timer::Clock::now();

        // ----------
        // 1. Gather user input and dispatch it.

        // Get picking results from last frame and forward it to the selection.
        m_viewer->processPicking();

        // ----------
        // 2. Kickoff rendering
        m_viewer->startRendering( dt );

        timerData.tasksStart = Core::Timer::Clock::now();

        // ----------
        // 3. Run the engine task queue.
        m_engine->getTasks( m_taskQueue.get(), dt );

        if (m_recordGraph) {m_taskQueue->printTaskGraph(std::cout);}

        // Run one frame of tasks
        m_taskQueue->startTasks();
        m_taskQueue->waitForTasks();
        timerData.taskData = m_taskQueue->getTimerData();
        m_taskQueue->flushTaskQueue();

        timerData.tasksEnd = Core::Timer::Clock::now();

        // ----------
        // 4. Wait until frame is fully rendered and display.
        m_viewer->waitForRendering();

        timerData.renderData = m_viewer->getRenderer()->getTimerData();

        // ----------
        // 5. Synchronize whatever needs synchronisation
        m_engine->endFrameSync();

        // ----------
        // 6. Frame end.
        timerData.frameEnd = Core::Timer::Clock::now();
        timerData.numFrame = m_frameCounter;

        if (m_recordTimings) { timerData.print(std::cout); }

        m_timerData.push_back( timerData );

        if (m_recordFrames)
        {
            recordFrame();
        }

        ++m_frameCounter;

        if (m_numFrames > 0  &&  m_frameCounter > m_numFrames )
        {
            appNeedsToQuit();
        }

        if ( m_frameCounter % m_frameCountBeforeUpdate == 0 )
        {
            emit( updateFrameStats( m_timerData ) );
            m_timerData.clear();
        }

        m_mainWindow->onFrameComplete();
    }

    void BaseApplication::appNeedsToQuit()
    {
        LOG( logDEBUG ) << "About to quit.";
        m_isAboutToQuit = true;
    }

    void BaseApplication::initializeOpenGlPlugins()
    {
        // Initialize plugins that depends on Initialized OpenGL (if any)
        if (m_openGLPlugins.size() > 0) {
            PluginContext context;
            context.m_engine = m_engine.get();
            context.m_selectionManager = m_mainWindow->getSelectionManager();
            context.m_pickingManager = m_viewer->getPickingManager();
            for (auto plugin : m_openGLPlugins)
            {
                plugin->openGlInitialize( context, m_viewer->getContext() );
            }
            m_openGLPlugins.clear();
        }
    }

    void BaseApplication::setRealFrameRate(bool on)
    {
       m_realFrameRate = on;
    }

    void BaseApplication::setRecordFrames(bool on)
    {
        m_recordFrames = on;
    }

    void BaseApplication::recordFrame()
    {
        std::string filename;
        Ra::Core::StringUtils::stringPrintf(filename, "%s/radiumframe_%06u.png", m_exportFoldername.c_str(), m_frameCounter);
        m_viewer->grabFrame(filename);
    }

    BaseApplication::~BaseApplication()
    {
        emit stopping();
        m_mainWindow->cleanup();
        m_engine->cleanup();

        // This will remove the directory if empty.
        QDir().rmdir( m_exportFoldername.c_str());

    }

    bool BaseApplication::loadPlugins( const std::string& pluginsPath, const QStringList& loadList, const QStringList& ignoreList )
    {
        QDir pluginsDir( qApp->applicationDirPath() );
        LOG( logINFO )<<" *** Loading Plugins ***";
        bool result = pluginsDir.cd( pluginsPath.c_str() );

        if (!result)
        {
            LOG(logERROR) << "Cannot open specified plugins directory "<<pluginsPath;
            return false;
        }

        LOG( logDEBUG )<<"Plugin directory :"<<pluginsDir.absolutePath().toStdString();
        bool res = true;
        uint pluginCpt = 0;

        PluginContext context;
        context.m_engine = m_engine.get();
        context.m_selectionManager = m_mainWindow->getSelectionManager();
        context.m_pickingManager = m_viewer->getPickingManager();

        for (const auto& filename : pluginsDir.entryList(QDir::Files))
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
                std::string basename = Core::StringUtils::getBaseName(filename.toStdString(),false);

                auto stringCmp = [basename](const QString& str) { return str.toStdString() == basename;};

                if (!loadList.empty() && std::find_if(loadList.begin(), loadList.end(),stringCmp ) == loadList.end() )
                {
                    LOG(logDEBUG)<<"Ignoring "<<filename.toStdString()<<" (not on load list)";
                    continue;
                }
                if ( std::find_if (ignoreList.begin(), ignoreList.end(), stringCmp) != ignoreList.end())
                {
                    LOG(logDEBUG)<<"Ignoring "<<filename.toStdString()<<" (on ignore list)";
                    continue;
                }

                QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( filename ) );
                // Force symbol resolution at load time.
                pluginLoader.setLoadHints( QLibrary::ResolveAllSymbolsHint );

                LOG( logINFO ) << "Found plugin " << filename.toStdString();

                QObject* plugin = pluginLoader.instance();
                Plugins::RadiumPluginInterface* loadedPlugin;

                if ( plugin )
                {
                    loadedPlugin = qobject_cast<Plugins::RadiumPluginInterface*>( plugin );
                    if ( loadedPlugin )
                    {
                        ++pluginCpt;
                        loadedPlugin->registerPlugin( context );
                        m_mainWindow->updateUi( loadedPlugin );

                        if(loadedPlugin->doAddRenderer())
                        {
                            std::vector<std::shared_ptr<Engine::Renderer>> tmpR;
                            loadedPlugin->addRenderers(&tmpR);
                            CORE_ASSERT(! tmpR.empty(), "This plugin is expected to add a renderer");
                            for(auto ptr : tmpR){
                                std::string name = ptr->getRendererName()
                                        + "(" + filename.toStdString() +  ")";
                                m_mainWindow->addRenderer(name, ptr);
                            }
                        }

                        if(loadedPlugin->doAddFileLoader())
                        {
                            std::vector<std::shared_ptr<Asset::FileLoaderInterface>> tmpL;
                            loadedPlugin->addFileLoaders(&tmpL);
                            CORE_ASSERT(! tmpL.empty(), "This plugin is expected to add file loaders");
                            for(auto ptr : tmpL){
                                m_engine->registerFileLoader(ptr);
                            }
                        }

                        if ( loadedPlugin->doAddROpenGLInitializer() )
                        {
                            if ( m_viewer->isOpenGlInitialized() )
                            {
                                LOG( logINFO ) << "Direct OpenGL initialization for plugin " << filename.toStdString();
                                // OpenGL is ready, initialize openGL part of the plugin
                                loadedPlugin->openGlInitialize( context, m_viewer->getContext() );
                            }
                            else
                            {
                                // Defer OpenGL initialisation
                                LOG( logINFO ) << "Defered OpenGL initialization for plugin " << filename.toStdString();
                                m_openGLPlugins.push_back(loadedPlugin);
                            }
                        }
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

        if (pluginCpt == 0)
        {
            LOG(logINFO) << "No plugin found or loaded.";
        }
        else
        {
            LOG(logINFO) << "Loaded " << pluginCpt << " plugins.";
        }

        return res;
    }

    void BaseApplication::setRecordTimings(bool on)
    {
        m_recordTimings = on;
    }

    void BaseApplication::setRecordGraph(bool on)
    {
        m_recordGraph = on;
    }
}
}
