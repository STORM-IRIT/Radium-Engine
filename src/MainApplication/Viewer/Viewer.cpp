#include <MainApplication/Viewer/Viewer.hpp>

#include <iostream>

#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/ComponentManager.hpp>
#include <Engine/Entity/EntityManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Drawable/DrawableComponent.hpp>
#include <Engine/Renderer/ForwardRenderer.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

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
    makeCurrent();
    initializeOpenGLFunctions();

    std::cerr<<"***Radium Engine Viewer***"<<std::endl;
    std::cerr<<"Renderer : " << glGetString(GL_RENDERER)<<std::endl;
    std::cerr<<"Vendor   : " << glGetString(GL_VENDOR)<<std::endl;
    std::cerr<<"OpenGL   : " << glGetString(GL_VERSION)<<std::endl;
    std::cerr<<"GLSL     : " << glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;

    m_engine = new Engine::RadiumEngine;
    m_engine->initialize();

    m_renderer = static_cast<Engine::RenderSystem*>(m_engine->getSystem("RenderSystem"));
    m_renderer->initializeGL(width(), height());
    m_engine->setupScene();

    m_engine->start();

    loadFile("../Scenes/spheres.dae");
    emit entitiesUpdated();
}

void Gui::Viewer::paintGL()
{
    makeCurrent();
    // TODO(Charly): Remove this, just temporary to ensure everything works fine.
//    static Scalar rotation = 0.0;
//    rotation += 0.01;

//    Entity* ent = EntityManager::getInstancePtr()->getEntity(0);
//    ent->setTransform(Transform(AngleAxis(std::sin(rotation), Vector3(0.0, 1.0, 0.0))));

    m_renderer->render();
}

void Gui::Viewer::resizeGL(int width, int height)
{
    makeCurrent();
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

    if (!m_engine->handleMouseEvent(e))
    {
        QOpenGLWidget::mousePressEvent(event);
    }
}

void Gui::Viewer::mouseReleaseEvent(QMouseEvent* event)
{
    Core::MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = Core::MouseEventType::RA_MOUSE_RELEASED;

    if (!m_engine->handleMouseEvent(e))
    {
        QOpenGLWidget::mouseReleaseEvent(event);
    }
}

void Gui::Viewer::mouseMoveEvent(QMouseEvent* event)
{
    Core::MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = Core::MouseEventType::RA_MOUSE_MOVED;

    if (!m_engine->handleMouseEvent(e))
    {
        QOpenGLWidget::mouseMoveEvent(event);
    }
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

    if (!m_engine->handleMouseEvent(e))
    {
        QOpenGLWidget::wheelEvent(event);
    }
}

void Gui::Viewer::keyPressEvent(QKeyEvent* event)
{
    Core::KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = Core::KeyEventType::RA_KEY_PRESSED;

    if (!m_engine->handleKeyEvent(e))
    {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void Gui::Viewer::keyReleaseEvent(QKeyEvent* event)
{
    Core::KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = Core::KeyEventType::RA_KEY_RELEASED;

    if (!m_engine->handleKeyEvent(e))
    {
        QOpenGLWidget::keyReleaseEvent(event);
    }
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
