#include <QApplication>
#include <QTimer>

#include <QOpenGLContext>

#include <GuiBase/Viewer/Viewer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>

#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <minimalradium.hpp>
/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */
#include <minimalapp.hpp>

int main(int argc, char* argv[])
{

    // Create default format for Qt.
    QSurfaceFormat format;
    format.setVersion( 4, 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    //format.setSamples( 16 );
    format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
    format.setSwapInterval( 0 );
    QSurfaceFormat::setDefaultFormat( format );

    // Create app
    LOG(logDEBUG) << "Creating application.";
    MinimalApp app(argc, argv);
    LOG(logDEBUG) << "Show viewer.";
    app.m_viewer->show();
    CORE_ASSERT( app.m_viewer->context()->isValid(), "OpenGL was not initialized" );

    LOG(logDEBUG) << "Creating BlinnPhong Shader.";
    // Load Blinn-Phong shader
    Ra::Engine::ShaderConfiguration bpConfig("BlinnPhong");
    bpConfig.addShader(Ra::Engine::ShaderType_VERTEX, "Shaders/BlinnPhong.vert.glsl");
    bpConfig.addShader(Ra::Engine::ShaderType_FRAGMENT, "Shaders/BlinnPhong.frag.glsl");
    Ra::Engine::ShaderConfigurationFactory::addConfiguration(bpConfig);

    // Create one system
    LOG(logDEBUG) << "Creating minimal system.";
    Ra::Engine::System* sys = new MinimalSystem;
    app.m_engine->registerSystem("Minimal system", sys);

    // Create and initialize entity and component
    LOG(logDEBUG) << "Creating an entity.";
    Ra::Engine::Entity* e = app.m_engine->getEntityManager()->createEntity("Cube");
    Ra::Engine::Component* c = new MinimalComponent;
    e->addComponent(c);
    sys->registerComponent(e, c);
    c->initialize();


    // Start the app.

    LOG(logDEBUG) << "Main loop.";
    app.m_frame_timer->start();
    return app.exec();
}
