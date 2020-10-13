#include <GuiBase/RadiumWindow/SimpleWindow.hpp>

#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>
#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <GuiBase/TreeModel/EntityTreeModel.hpp>
#include <GuiBase/Viewer/CameraManipulator.hpp>
#include <GuiBase/Viewer/Viewer.hpp>

namespace Ra {
using namespace Gui;
using namespace Engine;

namespace GuiBase {

SimpleWindow::SimpleWindow( QWidget* parent ) : MainWindowInterface( parent ) {
    if ( objectName().isEmpty() ) setObjectName( QString::fromUtf8( "RadiumSimpleWindow" ) );
    resize( 800, 640 );

    // Initialize the minimum tools for a Radium-Guibased Application
    m_viewer = std::make_unique<Viewer>();
    connect( m_viewer.get(), &Viewer::glInitialized, this, &SimpleWindow::onGLInitialized );
    m_viewer->setObjectName( QStringLiteral( "m_viewer" ) );

    // Initialize the scene interactive representation
    m_sceneModel = new Ra::GuiBase::ItemModel( Ra::Engine::RadiumEngine::getInstance(), this );
    m_selectionManager = new Ra::GuiBase::SelectionManager( m_sceneModel, this );

    // initialize Gui for the application
    auto viewerWidget = QWidget::createWindowContainer( m_viewer.get() );
    viewerWidget->setAutoFillBackground( false );
    setCentralWidget( viewerWidget );
    setWindowTitle( QString( "Radium player" ) );

    createConnections();
}

SimpleWindow::~SimpleWindow() = default;

Ra::Gui::Viewer* SimpleWindow::getViewer() {
    return m_viewer.get();
}

Ra::GuiBase::SelectionManager* SimpleWindow::getSelectionManager() {
    return m_selectionManager;
}

Ra::GuiBase::Timeline* SimpleWindow::getTimeline() {
    return nullptr;
}

void SimpleWindow::updateUi( Ra::Plugins::RadiumPluginInterface* ) {
    // no ui in the simple window, so, nothing to do
}

void SimpleWindow::onFrameComplete() {}

void SimpleWindow::addRenderer( const std::string&, std::shared_ptr<Ra::Engine::Renderer> e ) {
    m_viewer->addRenderer( e );
}

void SimpleWindow::postLoadFile( const std::string& ) {
    m_viewer->makeCurrent();
    m_viewer->getRenderer()->buildAllRenderTechniques();
    m_viewer->doneCurrent();
    m_selectionManager->clear();
    auto aabb = Ra::Engine::RadiumEngine::getInstance()->computeSceneAabb();
    if ( aabb.isEmpty() ) { m_viewer->getCameraManipulator()->resetCamera(); }
    else
    { m_viewer->fitCameraToScene( aabb ); }
    emit frameUpdate();
}

void SimpleWindow::cleanup() {
    m_viewer.reset( nullptr );
}

void SimpleWindow::createConnections() {}

void SimpleWindow::onGLInitialized() {
    auto forwardRenderer = std::make_shared<ForwardRenderer>();
    addRenderer( "Forward renderer (default)", forwardRenderer );
}

} // namespace GuiBase
} // namespace Ra
