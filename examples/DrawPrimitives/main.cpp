#include <Engine/RaEngine.hpp>

#include <QApplication>

#include <QOpenGLContext>

#include <Engine/Scene/EntityManager.hpp>

#include <minimalapp.hpp>
#include <minimalradium.hpp>

#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>

using namespace Ra::Gui;

int main( int argc, char* argv[] ) {

    // Create app and show viewer window
    MinimalApp app( argc, argv );
    app.initialize();

    // process all events so that everithing is initialized
    QApplication::processEvents();

    // Create and initialize entity and component
    Ra::Engine::Scene::Entity* e = app.m_engine->getEntityManager()->createEntity( "Cube" );
    MinimalComponent* c          = new MinimalComponent( e );
    c->initialize();

    // prepare the viewer to render the scene (i.e. build RenderTechniques for the active renderer)
    app.m_viewer->prepareDisplay();

    // Start the app.
    app.m_frameTimer->start();

    return app.exec();
}
