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

// FIXME (Charly) :
//  For now, we are just calling the Renderer::render() method here
//  We need to create a QThread, that will call periodically Engine::update() method.
//  Engine::update() will basically be a game loop, taking into account the different
//  update rates for each system, and so on...

namespace Ra
{

Gui::Viewer::Viewer(QWidget* parent)
    : QOpenGLWidget(parent)
{
    // Allow Viewer to receive events
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(QSize(800, 600));

    // TODO(Charly): Remove this, call update from the engine thread.
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000.0 / 60.0);
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

    m_engine = new Engine::RadiumEngine;
    m_engine->initialize();

    m_renderer = new Engine::Renderer(m_engine, width(), height());
    m_renderer->initialize();

    m_camera = new Engine::Camera;
//    m_engine->setupScene();

    m_engine->start();

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

bool Gui::Viewer::loadFile(const QString& path)
{
    makeCurrent();
    bool result = m_engine->loadFile(path.toStdString());
    doneCurrent();

    return result;
}

void Gui::Viewer::mousePressEvent(QMouseEvent* event)
{
    Core::MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = Core::MouseEventType::RA_MOUSE_PRESSED;

    m_renderer->handleMouseEvent(e);
    m_engine->handleMouseEvent(e);
    QOpenGLWidget::mousePressEvent(event);
}

void Gui::Viewer::mouseReleaseEvent(QMouseEvent* event)
{
    Core::MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = Core::MouseEventType::RA_MOUSE_RELEASED;

    m_renderer->handleMouseEvent(e);
    m_engine->handleMouseEvent(e);
    QOpenGLWidget::mouseReleaseEvent(event);
}

void Gui::Viewer::mouseMoveEvent(QMouseEvent* event)
{
    Core::MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = Core::MouseEventType::RA_MOUSE_MOVED;

    m_renderer->handleMouseEvent(e);
    m_engine->handleMouseEvent(e);
    QOpenGLWidget::mouseMoveEvent(event);
}

void Gui::Viewer::wheelEvent(QWheelEvent* event)
{
    Core::MouseEvent e;
    e.event = Core::MouseEventType::RA_MOUSE_WHEEL;
    e.wheelDelta = static_cast<Scalar>(event->delta());

    Scalar x = static_cast<Scalar>(event->x());
    Scalar y = static_cast<Scalar>(event->y());
    e.absoluteXPosition = x;
    e.absoluteYPosition = y;
    e.relativeXPosition = x / static_cast<Scalar>(width());
    e.relativeYPosition = y / static_cast<Scalar>(height());

    m_renderer->handleMouseEvent(e);
    m_engine->handleMouseEvent(e);
    QOpenGLWidget::wheelEvent(event);
}

void Gui::Viewer::keyPressEvent(QKeyEvent* event)
{
    Core::KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = Core::KeyEventType::RA_KEY_PRESSED;

    m_renderer->handleKeyEvent(e);
    m_engine->handleKeyEvent(e);
    QOpenGLWidget::keyPressEvent(event);
}

void Gui::Viewer::keyReleaseEvent(QKeyEvent* event)
{
    Core::KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = Core::KeyEventType::RA_KEY_RELEASED;

    m_renderer->handleKeyEvent(e);
    m_engine->handleKeyEvent(e);
    QOpenGLWidget::keyReleaseEvent(event);
}

void Gui::Viewer::mouseEventQtToRadium(QMouseEvent* qtEvent, Core::MouseEvent* raEvent)
{
    switch (qtEvent->button())
    {
        case Qt::LeftButton:
        {
            raEvent->button = Core::MouseButton::RA_MOUSE_LEFT_BUTTON;
        } break;

        case Qt::MiddleButton:
        {
            raEvent->button = Core::MouseButton::RA_MOUSE_MIDDLE_BUTTON;
        } break;

        case Qt::RightButton:
        {
            raEvent->button = Core::MouseButton::RA_MOUSE_RIGHT_BUTTON;
        } break;

        default:
        {
        } break;
    }

    raEvent->modifier = 0;

    if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        raEvent->modifier |= Core::Modifier::RA_CTRL_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        raEvent->modifier |= Core::Modifier::RA_SHIFT_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::AltModifier))
    {
        raEvent->modifier |= Core::Modifier::RA_ALT_KEY;
    }

    Scalar x = static_cast<Scalar>(qtEvent->x());
    Scalar y = static_cast<Scalar>(qtEvent->y());
    raEvent->absoluteXPosition = x;
    raEvent->absoluteYPosition = y;
    raEvent->relativeXPosition = x / static_cast<Scalar>(width());
    raEvent->relativeYPosition = y / static_cast<Scalar>(height());
}

void Gui::Viewer::keyEventQtToRadium(QKeyEvent* qtEvent, Core::KeyEvent* raEvent)
{
    raEvent->key = qtEvent->key();

    raEvent->modifier = 0;

    if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        raEvent->modifier |= Core::Modifier::RA_CTRL_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        raEvent->modifier |= Core::Modifier::RA_SHIFT_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::AltModifier))
    {
        raEvent->modifier |= Core::Modifier::RA_ALT_KEY;
    }
}

void Gui::Viewer::quit()
{
    m_engine->quit();
    fprintf(stderr, "About to quit... Cleaning RadiumEngine memory.\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

} // namespace Ra
