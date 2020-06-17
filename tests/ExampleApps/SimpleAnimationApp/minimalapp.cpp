#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>
#include <minimalapp.hpp>

#include <GuiBase/Utils/KeyMappingManager.hpp>
#include <GuiBase/Viewer/TrackballCameraManipulator.hpp>

MinimalApp::MinimalApp( int& argc, char** argv ) :
    QApplication( argc, argv ),
    m_engine( nullptr ),
    m_task_queue( nullptr ),
    m_viewer( nullptr ),
    m_frame_timer( nullptr ),
    m_target_fps( 60 ) {
    // Set application and organization names in order to ensure uniform
    // QSettings configurations.
    // \see http://doc.qt.io/qt-5/qsettings.html#QSettings-4
    QCoreApplication::setOrganizationName( "STORM-IRIT" );
    QCoreApplication::setApplicationName( "SimpleAnimation" );

    // Initialize Engine.
    m_engine.reset( Ra::Engine::RadiumEngine::createInstance() );
    m_engine->initialize();

    ///\todo update when a basic viewer is implemented ... (to call setupKeyMappingCallbacks)
    Ra::Gui::KeyMappingManager::createInstance();
    Ra::Gui::KeyMappingManager::getInstance()->addListener(
        Ra::Gui::TrackballCameraManipulator::configureKeyMapping );
    Ra::Gui::KeyMappingManager::getInstance()->addListener( Ra::Gui::Viewer::configureKeyMapping );

    // Initialize taskqueue.
    m_task_queue.reset( new Ra::Core::TaskQueue( std::thread::hardware_concurrency() - 1 ) );
    // Initialize viewer.
    m_viewer.reset( new Ra::Gui::Viewer );

    CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );
    connect( m_viewer.get(), &Ra::Gui::Viewer::glInitialized, this, &MinimalApp::onGLInitialized );

    // Initialize timer for the spinning cube.
    m_frame_timer = new QTimer( this );
    m_close_timer = new QTimer( this );
}

MinimalApp::~MinimalApp() {
    // need to clean up everithing before engine is cleaned up.
    m_task_queue.reset( nullptr );
    m_viewer.reset( nullptr );
    m_engine->cleanup();
    m_engine.reset( nullptr );
}

void MinimalApp::onGLInitialized() {
    std::shared_ptr<Ra::Engine::Renderer> e( new Ra::Engine::ForwardRenderer() );
    m_viewer->addRenderer( e );
    connect( m_frame_timer, &QTimer::timeout, this, &MinimalApp::frame );
    m_frame_timer->setInterval( 1000 / m_target_fps );
    connect( m_close_timer, &QTimer::timeout, this, &MinimalApp::quit );
    m_close_timer->setInterval( 10000 ); // 10 second before exit
}

void MinimalApp::frame() {
    // We use a fixed time step, but it is also possible
    // to check the time from last frame.
    const Scalar dt = 1.f / Scalar( m_target_fps );

    // Starts the renderer
    m_viewer->startRendering( dt );

    // Collect and run tasks
    m_engine->getTasks( m_task_queue.get(), dt );
    m_task_queue->startTasks();
    m_task_queue->waitForTasks();
    m_task_queue->flushTaskQueue();

    // Finish the frame
    m_viewer->swapBuffers();

    m_engine->endFrameSync();
}
