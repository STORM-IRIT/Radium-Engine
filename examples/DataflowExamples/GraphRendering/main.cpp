// Include Radium base application and its simple Gui

#include <Core/Asset/FileLoaderInterface.hpp>
#include <Core/Utils/Log.hpp>

#include <Engine/Rendering/ForwardRenderer.hpp>
#include <Engine/Scene/EntityManager.hpp>

#include <Dataflow/Rendering/Renderer/DataflowRenderer.hpp>

#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindow.hpp>
#include <Gui/Viewer/Viewer.hpp>

// Qt Widgets
#include <QtWidgets>

/* ----------------------------------------------------------------------------------- */
using namespace Ra::Core::Utils;
using namespace Ra::Engine;
using namespace Ra::Gui;
using namespace Ra::Dataflow::Rendering::Renderer;
using namespace Ra::Dataflow::Rendering;

/**
 * SimpleWindow for demonstration
 */
class DemoWindowFactory : public BaseApplication::WindowFactory
{
    std::vector<std::shared_ptr<Rendering::Renderer>> m_renderers;

    static void addFileMenu( MainWindowInterface* window ) {
        // Add a menu to load a scene
        auto fileMenu       = window->menuBar()->addMenu( "&File" );
        auto fileOpenAction = new QAction( "&Open...", window );
        fileOpenAction->setShortcuts( QKeySequence::Open );
        fileOpenAction->setStatusTip( "Open a file." );
        fileMenu->addAction( fileOpenAction );

        // Connect the menu
        auto openFile = [window]() {
            QString filter;
            QString allexts;
            auto engine = RadiumEngine::getInstance();
            for ( const auto& loader : engine->getFileLoaders() ) {
                QString exts;
                for ( const auto& e : loader->getFileExtensions() ) {
                    exts.append( QString::fromStdString( e ) + " " );
                }
                allexts.append( exts + " " );
                filter.append( QString::fromStdString( loader->name() ) + " (" + exts + ");;" );
            }
            // add a filter concatenating all the supported extensions
            filter.prepend( "Supported files (" + allexts + ");;" );

            // remove the last ";;" of the string
            filter.remove( filter.size() - 2, 2 );

            QSettings settings;
            auto path     = settings.value( "files/load", QDir::homePath() ).toString();
            auto pathList = QFileDialog::getOpenFileNames( window, "Open Files", path, filter );

            if ( !pathList.empty() ) {
                engine->getEntityManager()->deleteEntities();
                settings.setValue( "files/load", pathList.front() );
                engine->loadFile( pathList.front().toStdString() );
                engine->releaseFile();
                window->prepareDisplay();
                emit window->getViewer()->needUpdate();
            }
        };
        QAction::connect( fileOpenAction, &QAction::triggered, openFile );

        // Add an exit entry
        auto exitAct = fileMenu->addAction( "E&xit", window, &QWidget::close );
        exitAct->setShortcuts( QKeySequence::Quit );
    }

    static void
    addRendererMenu( MainWindowInterface* window,
                     const std::vector<std::shared_ptr<Rendering::Renderer>>& renderers ) {
        auto renderMenu = window->menuBar()->addMenu( "&Renderer" );
        int renderNum   = 0;

        for ( const auto& rndr : renderers ) {
            window->addRenderer( rndr->getRendererName(), rndr );
            auto rndAct = new QAction( rndr->getRendererName().c_str(), window );
            renderMenu->addAction( rndAct );
            QAction::connect( rndAct, &QAction::triggered, [renderNum, window]() {
                window->getViewer()->changeRenderer( renderNum );
                window->getViewer()->needUpdate();
            } );
            ++renderNum;
        }
    }

  public:
    explicit DemoWindowFactory(
        const std::vector<std::shared_ptr<Rendering::Renderer>>& renderers ) :
        m_renderers( renderers ) {}

    inline Ra::Gui::MainWindowInterface* createMainWindow() const override {
        auto window = new SimpleWindow();
        addFileMenu( window );
        addRendererMenu( window, m_renderers );
        return window;
    }
};
/* ----------------------------------------------------------------------------------- */
// Renderer controller
#include <Dataflow/Rendering/Nodes/Sources/Scene.hpp>

class MyRendererController : public DataflowRenderer::RenderGraphController
{
  public:
    using DataflowRenderer::RenderGraphController::RenderGraphController;
    /// Configuration function.
    /// Called once at the configuration of the renderer
    void configure( DataflowRenderer* renderer, int w, int h ) override {
        LOG( logINFO ) << "MyRendererController::configure";
        DataflowRenderer::RenderGraphController::configure( renderer, w, h );
        m_renderGraph = std::make_unique<RenderingGraph>( "Demonstration graph" );
        m_renderGraph->setShaderProgramManager( m_shaderMngr );
        auto sceneNode = new SceneNode( "Scene" );
        m_renderGraph->addNode( sceneNode );
    };

    /// Resize function
    /// Called each time the renderer is resized
    void resize( int w, int h ) override {
        LOG( logINFO ) << "MyRendererController::resize";
        DataflowRenderer::RenderGraphController::resize( w, h );
    };

    /// Update function
    /// Called once before each frame to update the internal state of the renderer
    void update( const Ra::Engine::Data::ViewingParameters& renderData ) override {
        LOG( logINFO ) << "MyRendererController::update";
        DataflowRenderer::RenderGraphController::update( renderData );
    };

    [[nodiscard]] std::string getRendererName() const override { return "Custom Node Renderer"; }
};

/* ----------------------------------------------------------------------------------- */

/**
 * main function.
 */
int main( int argc, char* argv[] ) {

    //! [Instatiating the application]
    BaseApplication app( argc, argv );
    //! [Instatiating the application]

    //! getting graph argument on the command line
    std::optional<std::string> graphOption { std::nullopt };
    QCommandLineParser parser;
    QCommandLineOption graphOpt(
        { "g", "graph", "nodes" }, "Open a node <graph> at startup.", "graph", "" );
    parser.addOptions( { graphOpt } );
    if ( !parser.parse( app.arguments() ) ) {
        LOG( Ra::Core::Utils::logWARNING )
            << "GraphDemo : Command line parsing failed due to unsupported or "
               "missing options : \n\t"
            << parser.errorText().toStdString();
    }
    if ( parser.isSet( graphOpt ) ) {
        graphOption = parser.value( graphOpt ).toStdString();
        std::cout << "Got a graph option : " << *graphOption << std::endl;
    }
    else {
        std::cout << "No graph option" << std::endl;
    }
    //! getting graph argument on the command line

    //! [Initializing the application]
    std::vector<std::shared_ptr<Rendering::Renderer>> renderers;
    renderers.emplace_back( new Rendering::ForwardRenderer );

    MyRendererController graphController;
    if ( graphOption ) { graphController.deferredLoadGraph( *graphOption ); }
    renderers.emplace_back( new DataflowRenderer( graphController ) );

    app.initialize( DemoWindowFactory( renderers ) );
    app.setContinuousUpdate( false );
    app.addRadiumMenu();
    //! [Initializing the application]

    return app.exec();
}
