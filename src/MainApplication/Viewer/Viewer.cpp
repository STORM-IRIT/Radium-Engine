#include <MainApplication/Viewer/Viewer.hpp>

#include <iostream>

#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>

#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/ComponentManager.hpp>
#include <Engine/Entity/EntityManager.hpp>
#include <Engine/Entity/Entity.hpp>

#include <Engine/Renderer/DrawableComponent.hpp>
#include <Engine/Renderer/ForwardRenderer.hpp>
#include <Engine/Renderer/Mesh.hpp>

// FIXME (Charly) :
//  For now, we are just calling the Renderer::render() method here
//  We need to create a QThread, that will call periodically Engine::update() method.
//  Engine::update() will basically be a game loop, taking into account the different
//  update rates for each system, and so on...

namespace Ra
{

Viewer::Viewer(QWidget* parent)
	: QOpenGLWidget(parent)
{
    // Allow Viewer to receive events
    setFocusPolicy(Qt::StrongFocus);

    // TODO(Charly): Remove this, call update from the engine thread.
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000.0 / 60.0);
}

Viewer::~Viewer()
{
}

void Viewer::initializeGL()
{
	makeCurrent();

	initializeOpenGLFunctions();

    std::cout<<"***Radium Engine Viewer***"<<std::endl;
    std::cout<<"Renderer : "<<glGetString(GL_RENDERER)<<std::endl;
    std::cout<<"Vendor : "<<glGetString(GL_VENDOR)<<std::endl;
    std::cout<<"OpenGL v."<<glGetString(GL_VERSION)<<std::endl;
    std::cout<<"GLSL v."<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;

    ComponentManager* cManager = ComponentManager::createInstance();
    EntityManager* eManager = EntityManager::createInstance();

    m_renderer = std::make_shared<ForwardRenderer>(width(), height());
    m_renderer->initialize();

    Mesh* mesh = new Mesh("Mesh");
    VertexData v0, v1, v2;
    v0.position = Vector3(-0.5, -0.5, 0);
    v1.position = Vector3(0, 0.5, 0);
    v2.position = Vector3(0.5, -0.5, 0);
    MeshData d;
    d.vertices = {v0, v1, v2};
    d.indices  = {0, 2, 1};
    mesh->loadGeometry(d);

    Entity* ent = eManager->createEntity();
    DrawableComponent* comp = new DrawableComponent();
    comp->addDrawable(mesh);

    cManager->addComponent(comp);
    ent->addComponent(comp);
    m_renderer->addComponent(comp);
}

void Viewer::paintGL()
{
	makeCurrent();
    // TODO(Charly): Remove this, just temporary to ensure everything works fine.
    static Scalar rotation = 0.0;
    rotation += 0.01;

    Entity* ent = EntityManager::getInstancePtr()->getEntity(0);
    ent->setTransform(Transform(AngleAxis(std::sin(rotation), Vector3(0.0, 1.0, 0.0))));

    m_renderer->update();
}

void Viewer::resizeGL(int width, int height)
{
	makeCurrent();
	m_renderer->resize(width, height);
}

void Viewer::mousePressEvent(QMouseEvent* event)
{
    MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = MouseEventType::MOUSE_PRESSED;

    if (!m_renderer->handleMouseEvent(e))
    {
        QOpenGLWidget::mousePressEvent(event);
    }
}

void Viewer::mouseReleaseEvent(QMouseEvent* event)
{
    MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = MouseEventType::MOUSE_RELEASED;

    if (!m_renderer->handleMouseEvent(e))
    {
        QOpenGLWidget::mouseReleaseEvent(event);
    }
}

void Viewer::mouseMoveEvent(QMouseEvent* event)
{
    MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = MouseEventType::MOUSE_MOVED;

    if (!m_renderer->handleMouseEvent(e))
    {
        QOpenGLWidget::mouseMoveEvent(event);
    }
}

void Viewer::wheelEvent(QWheelEvent* event)
{
    MouseEvent e;
    e.event = MouseEventType::MOUSE_WHEEL;
    e.wheelDelta = static_cast<Scalar>(event->delta());

    Scalar x = static_cast<Scalar>(event->x());
    Scalar y = static_cast<Scalar>(event->y());
    e.absoluteXPosition = x;
    e.absoluteYPosition = y;
    e.relativeXPosition = x / static_cast<Scalar>(width());
    e.relativeYPosition = y / static_cast<Scalar>(height());

    if (!m_renderer->handleMouseEvent(e))
    {
        QOpenGLWidget::wheelEvent(event);
    }
}

void Viewer::keyPressEvent(QKeyEvent* event)
{
    KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = KeyEventType::KEY_PRESSED;

    if (!m_renderer->handleKeyEvent(e))
    {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void Viewer::keyReleaseEvent(QKeyEvent* event)
{
    KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = KeyEventType::KEY_RELEASED;

    if (!m_renderer->handleKeyEvent(e))
    {
        QOpenGLWidget::keyReleaseEvent(event);
    }
}

void Viewer::mouseEventQtToRadium(QMouseEvent* qtEvent, MouseEvent* raEvent)
{
    switch (qtEvent->button())
    {
        case Qt::LeftButton:
        {
            raEvent->button = MouseButton::MOUSE_LEFT_BUTTON;
        } break;

        case Qt::MiddleButton:
        {
            raEvent->button = MouseButton::MOUSE_MIDDLE_BUTTON;
        } break;

        case Qt::RightButton:
        {
            raEvent->button = MouseButton::MOUSE_RIGHT_BUTTON;
        } break;

        default:
        {
        } break;
    }

    raEvent->modifier = 0;

    if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        raEvent->modifier |= Modifier::CTRL_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        raEvent->modifier |= Modifier::SHIFT_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::AltModifier))
    {
        raEvent->modifier |= Modifier::ALT_KEY;
    }

    Scalar x = static_cast<Scalar>(qtEvent->x());
    Scalar y = static_cast<Scalar>(qtEvent->y());
    raEvent->absoluteXPosition = x;
    raEvent->absoluteYPosition = y;
    raEvent->relativeXPosition = x / static_cast<Scalar>(width());
    raEvent->relativeYPosition = y / static_cast<Scalar>(height());
}

void Viewer::keyEventQtToRadium(QKeyEvent* qtEvent, KeyEvent* raEvent)
{
    raEvent->key = qtEvent->key();

    raEvent->modifier = 0;

    if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        raEvent->modifier |= Modifier::CTRL_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        raEvent->modifier |= Modifier::SHIFT_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::AltModifier))
    {
        raEvent->modifier |= Modifier::ALT_KEY;
    }
}

} // namespace Ra
