#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Rendering/ForwardRenderer.hpp>
#include <minimalapp.hpp>

#include <Gui/Utils/KeyMappingManager.hpp>
#include <Gui/Viewer/RotateAroundCameraManipulator.hpp>
#include <Gui/Viewer/TrackballCameraManipulator.hpp>

#include <QMainWindow>
#include <QOpenGLContext>

using namespace Ra;
using namespace Ra::Gui;

MinimalApp::MinimalApp( int& argc, char** argv ) : QApplication( argc, argv ) {

    QCoreApplication::setOrganizationName( "STORM-IRIT" );
    QCoreApplication::setApplicationName( "DrawPrimitives" );

    KeyMappingManager::createInstance();
}

MinimalApp::~MinimalApp() {
    // need to clean up everithing before engine is cleaned up.
    m_taskQueue.reset( nullptr );
    m_viewer.reset( nullptr );
    m_engine->cleanup();
    m_engine.reset( nullptr );
}

void MinimalApp::initialize() {
    QSurfaceFormat format;
    format.setVersion( 4, 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    format.setSamples( 16 );
    format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
    format.setSwapInterval( 0 );
    QSurfaceFormat::setDefaultFormat( format );

    // Initialize Engine.
    m_engine.reset( Engine::RadiumEngine::createInstance() );
    m_engine->initialize();

    // Initialize taskqueue.
    m_taskQueue.reset( new Core::TaskQueue( std::thread::hardware_concurrency() - 1 ) );

    // Initialize viewer.
    m_viewer.reset( new Viewer );
    CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );

    m_viewer->setupKeyMappingCallbacks();
    auto keyMappingManager = KeyMappingManager::getInstance();
    keyMappingManager->addListener(
        RotateAroundCameraManipulator::KeyMapping::configureKeyMapping );

    connect( m_viewer.get(),
             &Viewer::requestEngineOpenGLInitialization,
             this,
             &MinimalApp::onRequestEngineOpenGLInitialization );
    connect( m_viewer.get(), &Viewer::glInitialized, this, &MinimalApp::onGLInitialized );
    connect( this, &QGuiApplication::lastWindowClosed, m_viewer.get(), &WindowQt::cleanupGL );

    CORE_ASSERT( m_viewer->getContext() != nullptr, "OpenGL context was not created" );
    CORE_ASSERT( m_viewer->getContext()->isValid(), "OpenGL was not initialized" );

    // Initialize timer for the spinning cube.
    m_frameTimer = new QTimer( this );
    m_frameTimer->setInterval( 1000 / m_targetFps );

    // Create a window container for the viewer.
    // using viewer directly as main windows fail to have valid glbinding context
    // on Debian bullseye, Qt version 6.2.4
    auto viewerWidget = QWidget::createWindowContainer( m_viewer.get() );
    viewerWidget->setAutoFillBackground( false );
    viewerWidget->resize( 500, 500 );
    viewerWidget->show();
}

void MinimalApp::onGLInitialized() {
    // add the renderer
    std::shared_ptr<Engine::Rendering::Renderer> e( new Engine::Rendering::ForwardRenderer() );
    m_viewer->addRenderer( e );

    m_viewer->setCameraManipulator( new RotateAroundCameraManipulator(
        *( m_viewer->getCameraManipulator() ), m_viewer.get() ) );

    connect( m_frameTimer, &QTimer::timeout, this, &MinimalApp::frame );
}

void MinimalApp::onRequestEngineOpenGLInitialization() {
    // initialize here the OpenGL part of the engine used by the application
    m_engine->initializeGL();
}

void MinimalApp::frame() {
    // We use a fixed time step, but it is also possible
    // to check the time from last frame.
    const Scalar dt = 1.f / Scalar( m_targetFps );

    // Collect and run tasks
    m_engine->getTasks( m_taskQueue.get(), dt );
    m_taskQueue->startTasks();
    m_taskQueue->waitForTasks();
    m_taskQueue->flushTaskQueue();

    // Starts the renderer
    m_viewer->startRendering( dt );

    // Finish the frame
    m_viewer->swapBuffers();

    m_engine->endFrameSync();
}
