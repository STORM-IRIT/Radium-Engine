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
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Entity/Entity.hpp>
#include <MainApplication/Gui/MainWindow.hpp>

#include <Plugins/FancyMeshPlugin/FancyMeshSystem.hpp>
#include <Plugins/RigidBodyPlugin/RigidBodySystem.hpp>


// Const parameters : TODO : make config / command line options
const uint FPS_MAX = 60;

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
        //, m_timerData(TIMER_AVERAGE)
    {
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
        LOG( logINFO ) << "(Log using default file)";

        // Handle command line arguments.
        // TODO ( e.g fps limit ) / Keep or not timer data .

        // Create default format for Qt.
        QSurfaceFormat format;
        format.setVersion( 4, 4 );
        format.setProfile( QSurfaceFormat::CoreProfile );
        format.setSamples( 0 );
        format.setDepthBufferSize( 24 );
        format.setStencilBufferSize( 8 );
        format.setSamples( 16 );
        format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
        QSurfaceFormat::setDefaultFormat( format );

        // Create main window.
        m_mainWindow.reset( new Gui::MainWindow );
        m_mainWindow->show();

        // Allow all events to be processed (thus the viewer should have
        // initialized the OpenGL context..)
        processEvents();

        m_viewer = m_mainWindow->getViewer();
        CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );
        CORE_ASSERT( m_viewer->context()->isValid(), "OpenGL was not initialized" );

        // Create engine
        m_engine.reset( new Engine::RadiumEngine );
        m_engine->initialize();
        registerSystems();

        // Pass the engine to the renderer to complete the initialization process.
        m_viewer->initRenderer( m_engine.get() );

        // Create task queue with N-1 threads (we keep one for rendering).
        m_taskQueue.reset( new Core::TaskQueue( std::thread::hardware_concurrency() - 1 ) );

        createConnections();

        setupScene();
        emit starting();

        m_lastFrameStart = Core::Timer::Clock::now();
        m_frameTimer->start( 1000 / FPS_MAX );
    }

    void MainApplication::createConnections()
    {
        connect( m_frameTimer, SIGNAL( timeout() ), this, SLOT( radiumFrame() ) );
    }

    void MainApplication::registerSystems()
    {
        FancyMeshPlugin::FancyMeshSystem* fmSystem = new FancyMeshPlugin::FancyMeshSystem( m_engine.get() );
        m_engine->registerSystem( "FancyMeshSystem", fmSystem );

#if ENABLE_PHYSICS
        RigidBodyPlugin::RigidBodySystem* rbSystem = new RigidBodyPlugin::RigidBodySystem( m_engine.get() );
        m_engine->registerSystem( "RigidBodySystem", rbSystem );
#endif
    }

    void MainApplication::setupScene()
    {
        Engine::ShaderConfiguration shader( "BlinnPhong", "../Shaders" );

        Engine::Material* m0 = new Engine::Material( "m0" );
        m0->setKd( Core::Color( 1.0, 0.0, 0.0, 1.0 ) );
        m0->setKs( Core::Color( 0.0, 0.0, 0.0, 1.0 ) );
        m0->setMaterialType( Engine::Material::MaterialType::MAT_TRANSPARENT );
        Engine::RenderTechnique* r0 = new Engine::RenderTechnique;
        r0->shaderConfig = shader;
        r0->material = m0;

        Engine::Material* m1 = new Engine::Material( "m1" );
        m1->setKd( Core::Color( 0.0, 1.0, 0.0, 1.0 ) );
        m1->setKs( Core::Color( 0.0, 0.0, 0.0, 1.0 ) );
        //m1->setMaterialType ( Engine::Material::MaterialType::MAT_TRANSPARENT );
        Engine::RenderTechnique* r1 = new Engine::RenderTechnique;
        r1->shaderConfig = shader;
        r1->material = m1;

        Engine::Material* m2 = new Engine::Material( "m2" );
        m2->setKd( Core::Color( 0.0, 0.0, 1.0, 0.5 ) );
        m2->setKs( Core::Color( 0.0, 0.0, 0.0, 1.0 ) );
        m2->setMaterialType( Engine::Material::MaterialType::MAT_TRANSPARENT );
        Engine::RenderTechnique* r2 = new Engine::RenderTechnique;
        r2->shaderConfig = shader;
        r2->material = m2;

        Engine::Material* m3 = new Engine::Material( "m3" );
        m3->setKd( Core::Color( 1.0, 0.0, 1.0, 0.5 ) );
        m3->setKs( Core::Color( 0.0, 0.0, 0.0, 1.0 ) );
        m3->setMaterialType( Engine::Material::MaterialType::MAT_TRANSPARENT );
        Engine::RenderTechnique* r3 = new Engine::RenderTechnique;
        r3->shaderConfig = shader;
        r3->material = m3;

        Engine::Material* m4 = new Engine::Material( "m4" );
        m4->setKd( Core::Color( 1.0, 1.0, 0.0, 0.5 ) );
        m4->setKs( Core::Color( 0.0, 0.0, 0.0, 1.0 ) );
        m4->setMaterialType( Engine::Material::MaterialType::MAT_TRANSPARENT );
        Engine::RenderTechnique* r4 = new Engine::RenderTechnique;
        r4->shaderConfig = shader;
        r4->material = m4;

        Engine::Material* m5 = new Engine::Material( "m5" );
        m5->setKd( Core::Color( 0.0, 1.0, 1.0, 0.5 ) );
        m5->setKs( Core::Color( 0.0, 0.0, 0.0, 1.0 ) );
        m5->setMaterialType( Engine::Material::MaterialType::MAT_TRANSPARENT );
        Engine::RenderTechnique* r5 = new Engine::RenderTechnique;
        r5->shaderConfig = shader;
        r5->material = m5;

        Core::Transform transform;
        FancyMeshPlugin::FancyMeshSystem* fmSystem = static_cast<FancyMeshPlugin::FancyMeshSystem*>(
                                                         m_engine->getSystem( "FancyMeshSystem" ) );

        Engine::EntityManager* manager = m_engine->getEntityManager();

        //Engine::Entity* ent0 = manager->getOrCreateEntity( "box0" );
        //fmSystem->addFancyMeshToEntity( ent0, Core::MeshUtils::makeBox(), r0 );

        //transform.setIdentity();
        //transform.translation() = Core::Vector3( 2, 0, -3 );
        //ent0->setTransform( transform );

        Engine::Entity* ent1 = manager->getOrCreateEntity( "box1" );
        fmSystem->addFancyMeshToEntity( ent1, Core::MeshUtils::makeBox(), r1 );

        transform.setIdentity();
        transform.translation() = Core::Vector3( 0, 0, -3 );
        ent1->setTransform( transform );

        //Engine::Entity* ent2 = manager->getOrCreateEntity( "box2" );
        //fmSystem->addFancyMeshToEntity( ent2, Core::MeshUtils::makeBox(), r2 );

        //transform.setIdentity();
        //transform.translation() = Core::Vector3( -2, 0, -3 );
        //ent2->setTransform( transform );

        //Engine::Entity* ent3 = manager->getOrCreateEntity( "box3" );
        //fmSystem->addFancyMeshToEntity( ent3, Core::MeshUtils::makeBox(), r3 );

        //transform.setIdentity();
        //transform.translation() = Core::Vector3( 2, 0, -5 );
        //ent3->setTransform( transform );

        //Engine::Entity* ent4 = manager->getOrCreateEntity( "box4" );
        //fmSystem->addFancyMeshToEntity( ent4, Core::MeshUtils::makeBox(), r4 );

        //transform.setIdentity();
        //transform.translation() = Core::Vector3( 0, 0, -5 );
        //ent4->setTransform( transform );

        //Engine::Entity* ent5 = manager->getOrCreateEntity( "box5" );
        //fmSystem->addFancyMeshToEntity( ent5, Core::MeshUtils::makeBox(), r5 );

        //transform.setIdentity();
        //transform.translation() = Core::Vector3( -2, 0, -5 );
        //ent5->setTransform( transform );

#if ENABLE_PHYSICS
        RigidBodyPlugin::RigidBodySystem* rbSystem = static_cast<RigidBodyPlugin::RigidBodySystem*>(
                                                         m_engine->getSystem( "RigidBodySystem" ) );

        rbSystem->addRigidBodyToEntity( ent0, 1.0 );
        rbSystem->addRigidBodyToEntity( ent1, 1.0 );
        rbSystem->addRigidBodyToEntity( ent2, 1.0 );
        rbSystem->addRigidBodyToEntity( ent3, 1.0 );
        rbSystem->addRigidBodyToEntity( ent4, 1.0 );
        rbSystem->addRigidBodyToEntity( ent5, 1.0 );
#endif
    }

    void MainApplication::loadFile( QString path )
    {
        std::string pathStr = path.toLocal8Bit().data();
        bool res = m_engine->loadFile( pathStr );
        CORE_UNUSED( res );
        m_viewer->handleFileLoading( pathStr );
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
        auto picked = m_viewer->getRenderer()->getPickingResults();
        for ( auto pick : picked )
        {
            m_mainWindow->handlePicking( pick );
        }

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
        // 5. Frame end.
        timerData.frameEnd = Core::Timer::Clock::now();
        timerData.numFrame = m_frameCounter;

        m_timerData.push_back( timerData );
        ++m_frameCounter;

        if ( m_frameCounter % m_frameCountBeforeUpdate == 0 )
        {
            emit( updateFrameStats( m_timerData ) );
            m_timerData.clear();
        }
    }


    MainApplication::~MainApplication()
    {
        LOG( logINFO ) << "About to quit... Cleaning RadiumEngine memory";
        emit stopping();
        m_engine->cleanup();
    }
}
