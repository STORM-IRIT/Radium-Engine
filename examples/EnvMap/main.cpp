// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include <Gui/Viewer/Viewer.hpp>

// include the Engine/entity/component interface
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Scene/CameraManager.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>

// include the Environment texture management
#include <Core/Resources/Resources.hpp>
#include <Engine/Data/EnvironmentTexture.hpp>
// include the ForwardRenderer to add background rendering
#include <Engine/Rendering/ForwardRenderer.hpp>

// include Qt elements
#include <QTimer>

using namespace Ra::Core;
using namespace Ra::Engine;

//! [Implementing a simple custom renderer]
class SkyBoxRenderer : public Rendering::ForwardRenderer
{
  public:
    using ForwardRenderer::ForwardRenderer;
    std::string getRendererName() const override { return "Skybox Renderer"; }

    void setEnvMap( const std::string& filename ) {
        m_envMap.reset( new Data::EnvironmentTexture( filename, true ) );
    }

  protected:
    void renderBackground( const Data::ViewingParameters& viewData ) override {
        m_envMap->render( viewData );
    }

  private:
    std::unique_ptr<Data::EnvironmentTexture> m_envMap { nullptr };
};
//! [Implementing a simple custom renderer]

//! [Configuring a simple window with the custom renderer]
class DemoWindowFactory : public Ra::Gui::BaseApplication::WindowFactory
{
  public:
    ~DemoWindowFactory() = default;
    inline Ra::Gui::MainWindowInterface* createMainWindow() const override {
        auto window = new Ra::Gui::SimpleWindow();
        //! [Setting the custom Renderer]
        auto renderer = std::make_shared<SkyBoxRenderer>();
        window->addRenderer( renderer->getRendererName(), renderer );
        //! [Setting the custom Renderer]
        return window;
    }
};
//! [Configuring a simple window with the custom renderer]

int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    app.initialize( DemoWindowFactory {} );
    //! [Creating the application]

    //! [Setting the envmap on the renderer]
    auto rp            = Resources::getResourcesPath();
    std::string envmap = *rp + "/Examples/Assets/studio_garden_2k.exr";
    auto r = static_cast<SkyBoxRenderer*>( app.m_mainWindow->getViewer()->getRenderer() );

    r->setEnvMap( envmap );
    //! [Setting the envmap on the renderer]

    //! [Creating the cube]
    auto cube = Ra::Core::Geometry::makeSharpBox( { 0.1f, 0.1f, 0.1f } );
    //! [Creating the cube]

    //! [Colorize the Cube]
    cube.addAttrib(
        "in_color",
        Ra::Core::Vector4Array { cube.vertices().size(), Ra::Core::Utils::Color::Green() } );
    //! [Colorize the Cube]

    //! [Create the engine entity for the cube]
    auto e = app.m_engine->getEntityManager()->createEntity( "Green cube" );
    //! [Create the engine entity for the cube]

    //! [Create a geometry component with the cube]
    new Ra::Engine::Scene::TriangleMeshComponent( "Cube Mesh", e, std::move( cube ), nullptr );
    //! [Create a geometry component with the cube]

    //! [Place the camera away from the cube]
    {
        auto cameraManager =
            static_cast<Scene::CameraManager*>( app.m_engine->getSystem( "DefaultCameraManager" ) );
        auto c      = app.m_engine->getEntityManager()->createEntity( "Fixed Camera" );
        auto camera = new Scene::CameraComponent( c, "Camera" );
        camera->initialize();
        camera->getCamera()->setPosition( Vector3 { -2_ra, 1_ra, 5_ra } );
        camera->getCamera()->setDirection( Vector3 { 0.4_ra, -0.1_ra, -1_ra } );
        cameraManager->addCamera( camera );
        cameraManager->activate( cameraManager->getCameraIndex( camera ) );
    }
    //! [Place the camera away from the cube]

    //! [Tell the window that something is to be displayed]
    // Do not call app.m_mainWindow->prepareDisplay(); as it replace the active camera by the
    // default one
    app.m_mainWindow->getViewer()->makeCurrent();
    app.m_mainWindow->getViewer()->getRenderer()->buildAllRenderTechniques();
    app.m_mainWindow->getViewer()->doneCurrent();
    //! [Tell the window that something is to be displayed]

    // terminate the app after 8 second (approximatively). Camera can be moved using mouse moves.
    auto close_timer = new QTimer( &app );
    close_timer->setInterval( 8000 );
    QObject::connect( close_timer, &QTimer::timeout, [&app]() { app.appNeedsToQuit(); } );
    close_timer->start();

    return app.exec();
}
