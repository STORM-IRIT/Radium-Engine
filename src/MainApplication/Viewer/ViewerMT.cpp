#include <MainApplication/Viewer/ViewerMT.hpp>

#include <iostream>

#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QThread>
#include <QApplication>

#include <MainApplication/Viewer/EngineThread.hpp>

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

ViewerMT::ViewerMT(QWidget* parent)
    : QOpenGLWidget(parent)
{
    // Allow Viewer to receive events
    setFocusPolicy(Qt::StrongFocus);

    // TODO(Charly): Remove this, call update from the engine thread.
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000.0 / 60.0);
}

ViewerMT::~ViewerMT()
{
}

void ViewerMT::initializeGL()
{
    makeCurrent();

    initializeOpenGLFunctions();

    m_engineThread = new EngineThread(this, context()->currentContext(),
                                      context()->currentContext()->surface(), format());
    context()->moveToThread(m_engineThread);

    m_engineThread->initialize(width(), height());
    connect(m_engineThread, SIGNAL(finished()), m_engineThread, SLOT(deleteLater()));
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(clearEngine()));

    doneCurrent();
    m_engineThread->start();
}

void ViewerMT::clearEngine()
{
    fprintf(stderr, "About to quit, clearing engine stuff...\n");

    if (m_engineThread != nullptr && m_engineThread->isRunning())
    {
        m_engineThread->requestInterruption();
        m_engineThread->wait();
    }
}

void ViewerMT::paintGL()
{
    doneCurrent();
//	makeCurrent();
    // TODO(Charly): Remove this, just temporary to ensure everything works fine.
}

void ViewerMT::resizeGL(int width, int height)
{
    doneCurrent();
//	makeCurrent();
    m_engineThread->resize(width, height);
}

void ViewerMT::mousePressEvent(QMouseEvent* event)
{
    MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = MouseEventType::MOUSE_PRESSED;

    m_engineThread->mousePressEvent(event);

//    if (!m_renderer->handleMouseEvent(e))
//    {
//        QOpenGLWidget::mousePressEvent(event);
//    }


}

void ViewerMT::mouseReleaseEvent(QMouseEvent* event)
{
    MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = MouseEventType::MOUSE_RELEASED;

    m_engineThread->mouseReleaseEvent(event);

//    if (!m_renderer->handleMouseEvent(e))
//    {
//        QOpenGLWidget::mouseReleaseEvent(event);
//    }
}

void ViewerMT::mouseMoveEvent(QMouseEvent* event)
{
    MouseEvent e;
    mouseEventQtToRadium(event, &e);
    e.event = MouseEventType::MOUSE_MOVED;

//    if (!m_renderer->handleMouseEvent(e))
//    {
//        QOpenGLWidget::mouseMoveEvent(event);
//    }
}

void ViewerMT::wheelEvent(QWheelEvent* event)
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

//    if (!m_renderer->handleMouseEvent(e))
//    {
//        QOpenGLWidget::wheelEvent(event);
//    }
}

void ViewerMT::keyPressEvent(QKeyEvent* event)
{
    KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = KeyEventType::KEY_PRESSED;

//    if (!m_renderer->handleKeyEvent(e))
//    {
//        QOpenGLWidget::keyPressEvent(event);
//    }
}

void ViewerMT::keyReleaseEvent(QKeyEvent* event)
{
    KeyEvent e;
    keyEventQtToRadium(event, &e);
    e.event = KeyEventType::KEY_RELEASED;

//    if (!m_renderer->handleKeyEvent(e))
//    {
//        QOpenGLWidget::keyReleaseEvent(event);
//    }
}

void ViewerMT::mouseEventQtToRadium(QMouseEvent* qtEvent, MouseEvent* raEvent)
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

void ViewerMT::keyEventQtToRadium(QKeyEvent* qtEvent, KeyEvent* raEvent)
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
