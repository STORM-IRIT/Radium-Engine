// Include Radium base application and its simple Gui
#include "Gui/Utils/KeyMappingManager.hpp"
#include "Gui/Viewer/RotateAroundCameraManipulator.hpp"
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

// include the camera manager interface
#include <Engine/Scene/CameraManager.hpp>

// include the viewer to add key event
#include <Gui/Viewer/Viewer.hpp>

// To terminate the demo after a given time
#include <QTimer>
#include <qcoreevent.h>
#include <qevent.h>

using namespace Ra;
using namespace Ra::Core;
using namespace Ra::Engine;

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
        for ( auto e : m_animatedEntities ) {
            // Transform the entity
            q->registerTask( std::make_unique<Ra::Core::FunctionTask>(
                [e, t]() {
                    Transform T = e->getTransform();
                    T.translate(
                        Vector3 { std::cos( t ) * 0.025_ra, -std::sin( t ) * 0.025_ra, 0_ra } );
                    e->setTransform( T );
                },
                "move" ) );
        }
    }

  private:
    std::vector<Scene::Entity*> m_animatedEntities;
};
//! [Define a simple animation system]

/* ---------------------------------------------------------------------------------------------- */
/*                             main function that build the demo scene                            */
/* ---------------------------------------------------------------------------------------------- */
int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Gui::BaseApplication app( argc, argv );
    app.initialize( Ra::Gui::SimpleWindowFactory {} );
    //! [Creating the application]

    //![Parameterize the Engine  time loop]
    app.m_engine->setEndTime( std::numeric_limits<Scalar>::max() );
    app.m_engine->setRealTime( true );
    app.m_engine->play( true );
    //![Parameterize the Engine  time loop]

    //! [Cache the camera manager]
    auto cameraManager =
        static_cast<Scene::CameraManager*>( app.m_engine->getSystem( "DefaultCameraManager" ) );
    //! [Cache the camera manager]

    //! [Add usefull custom key events]
    auto callback = [cameraManager]( QEvent* event ) {
        if ( event->type() == QEvent::KeyPress ) {
            auto keyEvent = static_cast<QKeyEvent*>( event );
            // Convert ascii code to camera index
            cameraManager->activate( keyEvent->key() - '0' );
        }
    };
    app.m_mainWindow->getViewer()->addCustomAction(
        "switchCam0",
        Gui::KeyMappingManager::createEventBindingFromStrings( "", "ControlModifier", "Key_L" ),
        callback );
    app.m_mainWindow->getViewer()->addCustomAction(
        "switchCam1",
        Gui::KeyMappingManager::createEventBindingFromStrings( "", "ControlModifier", "Key_K" ),
        callback );
    //! [Add usefull custom key events]

    //! [Create the camera animation system demonstrator]
    auto animationSystem = new EntityAnimationSystem;
    app.m_engine->registerSystem( "Simple animation system", animationSystem );
    //! [Create the demo animation system]

    //! [Create the demo fixed entity/component]
    {
        //! [Create the engine entity for the fixed component]
        auto e = app.m_engine->getEntityManager()->createEntity( "Fixed cube" );
        //! [Create the engine entity for the fixed component]

        //! [Creating the cube]
        auto cube = Geometry::makeSharpBox( { 0.5f, 0.5f, 0.5f }, Utils::Color::Green() );
        //! [Creating the Cube]

        //! [Create a geometry component with the cube]
        // component ownership is transfered to entity in component ctor
        new Scene::TriangleMeshComponent( "Fixed cube geometry", e, std::move( cube ), nullptr );
        //! [Create a geometry component with the cube]
    }
    //! [Create the demo fixed entity/component]

    //! [Create the demo animated entity/components]
    {
        //! [Create the animated entity ]
        auto e = app.m_engine->getEntityManager()->createEntity( "Animated entity" );
        Transform transform( Translation { 1, 1, 0 } );
        e->setTransform( transform );
        e->swapTransformBuffers();
        //! [Create the animated entity ]

        //! [Register the entity to the animation system ]
        animationSystem->addEntity( e );
        //! [Register the entity to the animation system ]

        //! [attach components to the animated entity ]
        // an animated yellow cube
        auto cube =
            Geometry::makeSharpBox( { 0.1f, 0.1f, 0.1f }, Ra::Core::Utils::Color::Yellow() );
        new Scene::TriangleMeshComponent( "Fixed cube geometry", e, std::move( cube ), nullptr );

        // an animated camera, thay is not the one active at startup. Use key '0' to activate
        auto camera = new Scene::CameraComponent( e, "Animated Camera" );
        camera->initialize();
        camera->getCamera()->setPosition( Vector3 { 0.5, 0, 0 } );
        camera->getCamera()->setDirection( Vector3 { -1, 0, 0 } );
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
        camera->getCamera()->setPosition( Vector3 { 0, 0, 5 } );
        camera->getCamera()->setDirection( Vector3 { 0, 0, -1 } );
        camera->show( true );
        cameraManager->addCamera( camera );
        cameraManager->activate( cameraManager->getCameraIndex( camera ) );
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
