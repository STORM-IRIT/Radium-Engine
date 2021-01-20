#include <Gui/RadiumWindow/SimpleWindow.hpp>

#include <Engine/Renderer/ForwardRenderer.hpp>
#include <Gui/SelectionManager/SelectionManager.hpp>
#include <Gui/TreeModel/EntityTreeModel.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>

namespace Ra {
using namespace Gui;
using namespace Engine;

namespace Gui {

SimpleWindow::SimpleWindow( uint w, uint h, QWidget* parent ) : MainWindowInterface( parent ) {
    if ( objectName().isEmpty() ) setObjectName( QString::fromUtf8( "RadiumSimpleWindow" ) );
    resize( w, h );

    // Initialize the minimum tools for a Radium-Guibased Application
    m_viewer = std::make_unique<Viewer>();
    m_viewer->setObjectName( QStringLiteral( "m_viewer" ) );

    // Initialize the scene interactive representation
    m_sceneModel =
        std::make_unique<Ra::Gui::ItemModel>( Ra::Engine::RadiumEngine::getInstance(), this );
    m_selectionManager = std::make_unique<Ra::Gui::SelectionManager>( m_sceneModel.get(), this );

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

Ra::Gui::SelectionManager* SimpleWindow::getSelectionManager() {
    return m_selectionManager.get();
}

Ra::Gui::Timeline* SimpleWindow::getTimeline() {
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

} // namespace Gui
} // namespace Ra
