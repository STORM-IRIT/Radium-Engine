// Include Radium base application and its simple Gui

#include <Core/Asset/FileLoaderInterface.hpp>
#include <Core/Utils/Log.hpp>

#include <Engine/Rendering/ForwardRenderer.hpp>
#include <Engine/Scene/EntityManager.hpp>

#include <Dataflow/Rendering/Renderer/RenderGraphController.hpp>

#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindow.hpp>
#include <Gui/Viewer/Viewer.hpp>

#include <Dataflow/QtGui/GraphEditor/GraphEditorWindow.hpp>

// Qt Widgets
#include <QtWidgets>

/* ----------------------------------------------------------------------------------- */
using namespace Ra::Core::Utils;
using namespace Ra::Engine;
using namespace Ra::Gui;
using namespace Ra::Dataflow::Rendering::Renderer;
using namespace Ra::Dataflow::Rendering;

/**
 * Extending Ra::SimpleWindow with some menus for demonstration purpose
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
            std::cout << "adding renderer : " << rndr->getRendererName() << "\n";

            window->addRenderer( rndr->getRendererName(), rndr );
            auto rndAct = new QAction( rndr->getRendererName().c_str(), window );
            renderMenu->addAction( rndAct );
            QAction::connect( rndAct, &QAction::triggered, [renderNum, window]() {
                window->getViewer()->changeRenderer( renderNum );
                // window->getViewer()->needUpdate();
            } );

            // nodeToolBar* ff;
            // build a toolbar for the renderers
            if ( rndr->getRendererName() == "Custom Node Renderer" ) {
                auto nodeToolBar = window->addToolBar( "NodeGraph control" );
                nodeToolBar->setObjectName( "Edit Graph" );
                auto grphAction = new QAction( "Edit Graph", window );
                nodeToolBar->addAction( grphAction );
                nodeToolBar->hide();
                auto* cnfgRndr =
                    dynamic_cast<Ra::Dataflow::Rendering::Renderer::ControllableRenderer*>(
                        rndr.get() );

                QAction::connect( grphAction, &QAction::triggered, [cnfgRndr, window]() {
                    auto& nbrCtrl =
                        dynamic_cast<Ra::Dataflow::Rendering::Renderer::RenderGraphController&>(
                            cnfgRndr->getController() );
                    auto editor = new Ra::Dataflow::QtGui::GraphEditor::GraphEditorWindow(
                        nbrCtrl.getGraph() );
                    Ra::Dataflow::QtGui::GraphEditor::GraphEditorWindow::connect(
                        editor,
                        &Ra::Dataflow::QtGui::GraphEditor::GraphEditorWindow::needUpdate,
                        [window]() { window->getViewer()->needUpdate(); } );
                    editor->show();
                } );
                QAction::connect(
                    rndAct, &QAction::triggered, [nodeToolBar]() { nodeToolBar->show(); } );
            }
            else {
                QAction::connect( rndAct, &QAction::triggered, [window]() {
                    auto toolbars = window->findChildren<QToolBar*>();
                    for ( auto tb : toolbars ) {
                        if ( tb->objectName() == "Edit Graph" ) { tb->hide(); }
                    }
                } );
            }

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
#include <Dataflow/Rendering/Nodes/RenderNodes/ClearColorNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/GeometryAovsNode.hpp>
#include <Dataflow/Rendering/Nodes/RenderNodes/SimpleRenderNode.hpp>
#include <Dataflow/Rendering/Nodes/Sinks/DisplaySinkNode.hpp>

#include <Dataflow/Rendering/Nodes/Sources/Scene.hpp>
#include <Dataflow/Rendering/Nodes/Sources/TextureSourceNode.hpp>

class MyRendererController : public RenderGraphController
{
  private:
    static void inspectGraph( DataflowGraph& g ) {
        // Factories used by the graph
        auto factories = g.getNodeFactories();
        std::cout << "Used factories by the graph \"" << g.getInstanceName() << "\" with type \""
                  << g.getTypeName() << "\" :\n";
        for ( const auto& f : *( factories.get() ) ) {
            std::cout << "\t" << f.first << "\n";
        }

        // Nodes of the graph
        auto nodes = g.getNodes();
        std::cout << "Nodes of the graph " << g.getInstanceName() << " (" << nodes->size()
                  << ") :\n";
        for ( const auto& n : *( nodes ) ) {
            std::cout << "\t\"" << n->getInstanceName() << "\" of type \"" << n->getTypeName()
                      << "\"\n";
            // Inspect input, output and interfaces of the node
            std::cout << "\t\tInput ports :\n";
            for ( const auto& p : n->getInputs() ) {
                std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName()
                          << "\n";
            }
            std::cout << "\t\tOutput ports :\n";
            for ( const auto& p : n->getOutputs() ) {
                std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName()
                          << "\n";
            }
            std::cout << "\t\tInterface ports :\n";
            for ( const auto& p : n->getInterfaces() ) {
                std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName()
                          << "\n";
            }
        }

        // Nodes by level after the compilation
        auto c = g.compile();
        if ( c ) {
            auto cn = g.getNodesByLevel();
            std::cout << "Nodes of the graph, sorted by level when compiling the graph :\n";
            for ( size_t i = 0; i < cn->size(); ++i ) {
                std::cout << "\tLevel " << i << " :\n";
                for ( const auto n : ( *cn )[i] ) {
                    std::cout << "\t\t\"" << n->getInstanceName() << "\"\n";
                }
            }

            // describe the graph interface : inputs and outputs port of the whole graph (not of the
            // nodes)
            std::cout << "Inputs and output nodes of the graph " << g.getInstanceName() << " :\n";
            auto inputs = g.getAllDataSetters();
            std::cout << "\tInput ports (" << inputs.size() << ") are :\n";
            for ( auto& [ptrPort, portName, portType] : inputs ) {
                std::cout << "\t\t\"" << portName << "\" accepting type \"" << portType << "\"\n";
            }
            auto outputs = g.getAllDataGetters();
            std::cout << "\tOutput ports (" << outputs.size() << ") are :\n";
            for ( auto& [ptrPort, portName, portType] : outputs ) {
                std::cout << "\t\t\"" << portName << "\" generating type \"" << portType << "\"\n";
            }
        }
        else {
            std::cerr << "Unable to compile the graph " << g.getInstanceName() << "\n";
        }
    }

  public:
    using RenderGraphController::RenderGraphController;

    /// Configuration function.
    /// Called once at the configuration of the renderer
    /// If a graph should be loaded at configure time, it was set on the controller using
    /// deferredLoadGraph(...) before configuring the renderer.
    void configure( ControllableRenderer* renderer, int w, int h ) override {
        LOG( logINFO ) << "MyRendererController::configure";
        RenderGraphController::configure( renderer, w, h );
        if ( m_renderGraph == nullptr ) {
            // a graph was not given on the command line, build a simple one
            m_renderGraph = std::make_unique<RenderingGraph>( "Demonstration graph" );
            m_renderGraph->setShaderProgramManager( m_shaderMngr );
            auto sceneNode = new SceneNode( "Scene" );
            m_renderGraph->addNode( std::unique_ptr<Node>( sceneNode ) );
            auto resultNode = new DisplaySinkNode( "Images" );
            m_renderGraph->addNode( std::unique_ptr<Node>( resultNode ) );
            auto geomAovs = new GeometryAovsNode( "Geometry Aovs" );
            m_renderGraph->addNode( std::unique_ptr<Node>( geomAovs ) );
#if 0
            auto textureSource = new ColorTextureNode( "Beauty" );
            m_renderGraph->addNode( std::unique_ptr<Node>( textureSource ) );
            auto clearNode = new ClearColorNode( " Clear" );
            m_renderGraph->addNode( std::unique_ptr<Node>( clearNode ) );

            bool linksOK = true;
            linksOK      = m_renderGraph->addLink(
                textureSource, "texture", clearNode, "colorTextureToClear" );
            linksOK = linksOK && m_renderGraph->addLink( clearNode, "image", resultNode, "Beauty" );
#endif
            auto simpleRenderNode = new SimpleRenderNode( "renderOperator" );
            m_renderGraph->addNode( std::unique_ptr<Node>( simpleRenderNode ) );
            bool linksOK = true;
            linksOK      = linksOK &&
                      m_renderGraph->addLink( sceneNode, "objects", simpleRenderNode, "objects" );
            linksOK = linksOK &&
                      m_renderGraph->addLink( sceneNode, "camera", simpleRenderNode, "camera" );
            linksOK = linksOK &&
                      m_renderGraph->addLink( sceneNode, "lights", simpleRenderNode, "lights" );
            linksOK = linksOK &&
                      m_renderGraph->addLink( simpleRenderNode, "Beauty", resultNode, "Beauty" );
            linksOK = linksOK &&
                      m_renderGraph->addLink( simpleRenderNode, "Depth AOV", resultNode, "AOV_0" );
            linksOK =
                linksOK && m_renderGraph->addLink( geomAovs, "world normal", resultNode, "AOV_1" );
            linksOK = linksOK && m_renderGraph->addLink( sceneNode, "camera", geomAovs, "camera" );
            linksOK =
                linksOK && m_renderGraph->addLink( sceneNode, "objects", geomAovs, "objects" );

            if ( !linksOK ) { LOG( logERROR ) << "Something went wrong when linking nodes !!! "; }
            else {
                LOG( logINFO ) << "Graph linked successfully!!! ";
            }

            inspectGraph( *m_renderGraph );

            // force recompilation and introspection of the graph by the renderer
            m_renderGraph->needsRecompile();
            notify();
        }
    };

#if 0
    /// Resize function
    /// Called each time the renderer is resized
    void resize( int w, int h ) override {
        LOG( logINFO ) << "MyRendererController::resize";
        RenderGraphController::resize( w, h );
    };

    /// Update function
    /// Called once before each frame to update the internal state of the renderer
    void update( const Ra::Engine::Data::ViewingParameters& renderData ) override {
        LOG( logINFO ) << "MyRendererController::update";
        RenderGraphController::update( renderData );
    };
#endif
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
    renderers.emplace_back( new ControllableRenderer( graphController ) );

    app.initialize( DemoWindowFactory( renderers ) );
    app.setContinuousUpdate( false );
    app.addRadiumMenu();
    //! [Initializing the application]

    return app.exec();
}
