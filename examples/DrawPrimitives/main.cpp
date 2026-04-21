#include <Engine/RaEngine.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include <Gui/Viewer/RotateAroundCameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>

#include <QApplication>
#include <QOpenGLContext>

#include <AllPrimitivesComponent.hpp>
#include <minimalapp.hpp>

int main( int argc, char* argv[] ) {
    MinimalApp app( argc, argv );
    glbinding::Version glVersion { 4, 4 };
    app.initialize( glVersion );

    // process all events so that everithing is initialized
    QApplication::processEvents();

    if ( glVersion != app.m_engine->getOpenGLVersion() ) {
        LOG( Ra::Core::Utils::logWARNING )
            << "OpenGL version mismatch : requested " << glVersion.toString() << " -- available "
            << app.m_engine->getOpenGLVersion().toString() << std::endl;
    }

    // Create and initialize entity and component
    /// \todo Create one entity per object, instead of using the big "all primitive component"
    Ra::Engine::Scene::Entity* e =
        app.m_engine->getEntityManager()->createEntity( "All Primitives" );
    AllPrimitivesComponent* c = new AllPrimitivesComponent( e );
    c->initialize();

    auto viewer = app.m_viewer.get();
    viewer->prepareDisplay();
    app.m_frameTimer->start();
    return app.exec();
}
