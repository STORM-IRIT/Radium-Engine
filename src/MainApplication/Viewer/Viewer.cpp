#include <MainApplication/Viewer/Viewer.hpp>


#include <iostream>

#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/String/StringUtils.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/ComponentManager.hpp>
#include <Engine/Entity/EntityManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

#include <MainApplication/Gui/MainWindow.hpp>

// FIXME (Charly) :
//  For now, we are just calling the Renderer::render() method here
//  We need to create a QThread, that will call periodically Engine::update() method.
//  Engine::update() will basically be a game loop, taking into account the different
//  update rates for each system, and so on...

namespace
{
    Ra::Gui::MainWindow * getMainWin(const QWidget* w)
    {
        //Assumption : main window is our grand parent. This is checked in MainApplication
        return static_cast<Ra::Gui::MainWindow*>(w->parent()->parent());
    }
}
    namespace Ra
{

Gui::Viewer::Viewer(QWidget* parent)
    : QOpenGLWidget(parent)
{
    // Allow Viewer to receive events
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(QSize(800, 600));

}

Gui::Viewer::~Viewer()
{
}

void Gui::Viewer::initializeGL()
{
    initializeOpenGLFunctions();

    std::cerr<<"***Radium Engine Viewer***"<<std::endl;
    std::cerr<<"Renderer : " << glGetString(GL_RENDERER)<<std::endl;
    std::cerr<<"Vendor   : " << glGetString(GL_VENDOR)<<std::endl;
    std::cerr<<"OpenGL   : " << glGetString(GL_VERSION)<<std::endl;
    std::cerr<<"GLSL     : " << glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;


#if defined (OS_WINDOWS)
    glewExperimental = GL_TRUE;
    GLuint result = glewInit();
    if (result != GLEW_OK)
    {
        std::string errorStr;
        Ra::Core::StringUtils::stringPrintf(errorStr, " GLEW init failed : %s", glewGetErrorString(result));
        CORE_ERROR(errorStr.c_str());
    }
    else
    {
        std::cout << "GLEW v." << glewGetString(GLEW_VERSION) << std::endl;
    }
#endif

    m_renderer = new Engine::Renderer(width(), height());
    m_renderer->initialize();

    m_camera = new Engine::Camera;
//    m_engine->setupScene();
    emit ready(this);
  //  m_engine->start();

//    loadFile("../Scenes/stanford_dragon/dragon.obj");

//    Core::TriangleMesh d = Core::MeshUtils::makeGeodesicSphere(5, 1);
//    Engine::Entity* ent = m_engine->createEntity();
//    Engine::FancyMeshDrawable* comp = new Engine::FancyMeshDrawable;
//    m_engine->addComponent(comp, ent, "RenderSystem");
//    Engine::Mesh* mesh = new Engine::Mesh("mesh");
//    mesh->loadGeometry(d);
//    comp->addDrawable(mesh);
//    Engine::Material* mat = new Engine::Material("material");


//    const Engine::ShaderConfiguration defaultShaderConf("BlinnPhong", "../Shaders");
//    const Engine::ShaderConfiguration contourShaderConf("BlinnPhongContour", "../Shaders",
//                                                        Engine::ShaderConfiguration::DEFAULT_SHADER_PROGRAM_W_GEOM);
//    const Engine::ShaderConfiguration wireframeShaderConf("BlinnPhongWireframe", "../Shaders",
//                                                          Engine::ShaderConfiguration::DEFAULT_SHADER_PROGRAM_W_GEOM);

//    Engine::ShaderProgramManager* m = Engine::ShaderProgramManager::getInstancePtr();
//    mat->setDefaultShaderProgram(m->getShaderProgram(defaultShaderConf));
//    mat->setContourShaderProgram(m->getShaderProgram(contourShaderConf));
//    mat->setWireframeShaderProgram(m->getShaderProgram(wireframeShaderConf));
//    comp->setMaterial(mat);

//    emit entitiesUpdated();
}
void Gui::Viewer::paintGL()
{
    // TODO(Charly): Setup data, camera handled there.
    Engine::RenderData data;
    m_renderer->render(data);
}

void Gui::Viewer::resizeGL(int width, int height)
{
    m_renderer->resize(width, height);
}

void Gui::Viewer::mousePressEvent(QMouseEvent* event)
{
    QOpenGLWidget::mousePressEvent(event);
    getMainWin(this)->viewerMousePressEvent(event);

}
void Gui::Viewer::mouseReleaseEvent(QMouseEvent* event)
{
    QOpenGLWidget::mouseReleaseEvent(event);
    getMainWin(this)->viewerMouseReleaseEvent(event);
}

void Gui::Viewer::mouseMoveEvent(QMouseEvent* event)
{
    QOpenGLWidget::mouseMoveEvent(event);
    getMainWin(this)->viewerMouseMoveEvent(event);
}

void Gui::Viewer::wheelEvent(QWheelEvent* event)
{
    QOpenGLWidget::wheelEvent(event);
    getMainWin(this)->viewerWheelEvent(event);
}

} // namespace Ra
