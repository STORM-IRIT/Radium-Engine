// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>

// include the core geometry/appearance interface
#include <Core/Geometry/MeshPrimitives.hpp>

// include the Engine/entity/component/system/animation interface
#include <Engine/FrameInfo.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/System.hpp>

// include the task animation interface
#include <Core/Tasks/Task.hpp>

// include the Camera and default camera manager interface
#include <Engine/Scene/CameraComponent.hpp>
#include <Engine/Scene/DefaultCameraManager.hpp>

// include the viewer to activate the camera
#include <Gui/Viewer/CameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>

// To terminate the demo after 4 seconds
#include <QTimer>

using namespace Ra;
using namespace Ra::Core;
using namespace Ra::Engine;

/* ---------------------------------------------------------------------------------------------- */
/*                      Custom window with some key events to switch from camera                  */
/* ---------------------------------------------------------------------------------------------- */
/**
 * Simple custom windows for custom KeyEvent demonstration
 */
class DemoWindow : public Ra::Gui::SimpleWindow
{
    Q_OBJECT

  public:
    /// Reuse the SimpleWindow constructors
    using Ra::Gui::SimpleWindow::SimpleWindow;

    explicit DemoWindow( uint w = 800, uint h = 640, QWidget* parent = nullptr ) :
        Ra::Gui::SimpleWindow::SimpleWindow( w, h, parent ) {}

    //! [Configure Custom KeyEvent for the window]
    void configure() override {
        // <alt>+F will switch to default camera
        // <alt>+C will toggle on cameras
        // these customs events are only here for demonstration purposse.
        // events expressed for an apple french keyboard
        getViewer()->addKeyPressEventAction(
            "switchCam0", "Key_0", "ShiftModifier", "", "false", [this]( QKeyEvent* event ) {
                this->switchCamera( event );
            } );
        getViewer()->addKeyPressEventAction(
            "switchCam1", "Key_1", "ShiftModifier", "", "false", [this]( QKeyEvent* event ) {
                this->switchCamera( event );
            } );
    }
    //! [Configure Custom KeyEvent for the window]

    //! [Manage Custom KeyEvent reaching the window]
    void switchCamera( QKeyEvent* e ) {
        auto cameraManager = static_cast<Scene::DefaultCameraManager*>(
            Engine::RadiumEngine::getInstance()->getSystem( "DefaultCameraManager" ) );
        // Activating a camera require 2 things :
        // ask the camera manager to activate the camera
        cameraManager->activate( e->key() - '0' );
        // ask the CameraManipulator to update its camera info
        getViewer()->getCameraManipulator()->updateCamera();
    }
    //! [Manage Custom KeyEvent reaching the window]
};

/**
 * Define a factory that instantiates the Demonstration Window
 */
class DemoWindowFactory : public Ra::Gui::BaseApplication::WindowFactory
{
  public:
    ~DemoWindowFactory() = default;
    inline Ra::Gui::MainWindowInterface* createMainWindow() const override {
        auto window = new DemoWindow();
        return window;
    }
};
#include "main.moc"

/* ---------------------------------------------------------------------------------------------- */
/*                           Simple animation system to move an entity                            */
/* ---------------------------------------------------------------------------------------------- */
//! [Define a simple animation system]
/// This system will be added to the engine.
/// Every frame it will add a task to update the transformation of the attached entities.
class EntityAnimationSystem : public Scene::System
{
  public:
    void addEntity( Scene::Entity* e ) { m_animatedEntities.push_back( e ); }

    virtual void generateTasks( TaskQueue* q, const FrameInfo& info ) override {

        // get the entity of the first component
        auto t = info.m_animationTime;
        for ( auto e : m_animatedEntities )
        {
            // Transform the entity
            q->registerTask( new Ra::Core::FunctionTask(
                [e, t]() {
                    Transform T = e->getTransform();
                    T.translate( Vector3 {std::cos( t ) * 0.025, -std::sin( t ) * 0.025, 0_ra} );
                    e->setTransform( T );
                },
                "move" ) );
        }
    }

  private:
    std::vector<Scene::Entity*> m_animatedEntities;
};

/* ---------------------------------------------------------------------------------------------- */
/*                             main function that build the demo scene                            */
/* ---------------------------------------------------------------------------------------------- */
int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Gui::BaseApplication app( argc, argv );
    app.initialize( DemoWindowFactory {} );
    //! [Creating the application]

    //![Parameterize the Engine  time loop]
    app.m_engine->setEndTime( std::numeric_limits<Scalar>::max() );
    app.m_engine->setRealTime( true );
    app.m_engine->play( true );
    //![Parameterize the Engine  time loop]

    //! [Cache the camera manager]
    auto cameraManager = static_cast<Scene::DefaultCameraManager*>(
        app.m_engine->getSystem( "DefaultCameraManager" ) );
    //! [Cache the camera manager]

    //! [Create the camera animation system demonstrator]
    auto animationSystem = new EntityAnimationSystem;
    app.m_engine->registerSystem( "Simple animation system", animationSystem );
    //! [Create the demo animation system]

    //! [Create the demo fixed component]
    {
        //! [Create the engine entity for the fixed component]
        auto e = app.m_engine->getEntityManager()->createEntity( "Fixed cube" );
        //! [Create the engine entity for the fixed component]

        //! [Creating the cube]
        auto cube = Geometry::makeSharpBox( {0.5f, 0.5f, 0.5f}, Utils::Color::Green() );
        //! [Creating the Cube]

        //! [Create a geometry component with the cube]
        auto c = new Scene::TriangleMeshComponent(
            "Fixed cube geometry", e, std::move( cube ), nullptr );
        //! [Create a geometry component with the cube]
    }
    //! [Create the demo fixed component]

    //! [Create the demo animated entity/components]
    {
        //! [Create the animated entity ]
        auto e = app.m_engine->getEntityManager()->createEntity( "Animated entity" );
        Transform transform( Translation {1, 1, 0} );
        e->setTransform( transform );
        e->swapTransformBuffers();
        //! [Create the animated entity ]

        //! [Register the entity to the animation system ]
        animationSystem->addEntity( e );
        //! [Register the entity to the animation system ]

        //! [attach components to the animated entity ]
        // an animated yellow cube
        auto cube = Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f}, Ra::Core::Utils::Color::Yellow() );
        new Scene::TriangleMeshComponent( "Fixed cube geometry", e, std::move( cube ), nullptr );

        // an animated camera, thay is not the one active at startup. Use key '0' to activate
        auto camera = new Scene::CameraComponent( e, "Animated Camera" );
        camera->initialize();
        camera->getCamera()->setPosition( Vector3 {0.5, 0, 0} );
        camera->getCamera()->setDirection( Vector3 {-1, 0, 0} );
        camera->show( true );
        cameraManager->addCamera( camera );
        //! [attach components to the animated entity ]
    }
    //! [Create the demo animated entity/components]

    //! [Create the fixed reference camera]
    {
        auto e      = app.m_engine->getEntityManager()->createEntity( "Fixed Camera" );
        auto camera = new Scene::CameraComponent( e, "Camera" );
        camera->initialize();
        camera->getCamera()->setPosition( Vector3 {0, 0, 5} );
        camera->getCamera()->setDirection( Vector3 {0, 0, -1} );
        camera->show( true );
        cameraManager->addCamera( camera );
        // Activating a camera require 2 things :
        // ask the camera manager to activate the camera
        cameraManager->activate( cameraManager->getCameraIndex( camera ) );
        // ask the CameraManipulator to update its camera info
        app.m_mainWindow->getViewer()->getCameraManipulator()->updateCamera();
    }
    //! [Create the fixed reference camera]

    //! [Tell the window that something is to be displayed]
    // Do not call app.m_mainWindow->prepareDisplay(); as it replace the active camera by the
    // default one
    app.m_mainWindow->getViewer()->makeCurrent();
    app.m_mainWindow->getViewer()->getRenderer()->buildAllRenderTechniques();
    app.m_mainWindow->getViewer()->doneCurrent();
    //! [Tell the window that something is to be displayed]

#if 0
    // terminate the app after 30 second (approximatively).
    // All interactions on the viewer can be done when the application executes.
    auto close_timer = new QTimer( &app );
    close_timer->setInterval( 30000 );
    QObject::connect( close_timer, &QTimer::timeout, [&app]() { app.appNeedsToQuit(); } );
    close_timer->start();
#endif

    return app.exec();
}
