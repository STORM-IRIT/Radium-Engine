#include <MainApplication/Viewer/EngineThread.hpp>

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QSurface>

#include <Engine/Renderer/ForwardRenderer.hpp>
#include <Engine/Entity/EntityManager.hpp>
#include <Engine/Entity/ComponentManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/DrawableComponent.hpp>
#include <Engine/Renderer/Mesh.hpp>
#include <MainApplication/Viewer/Viewer.hpp>

namespace Ra
{

EngineThread::EngineThread(ViewerMT* viewer, QOpenGLContext* context,
                           QSurface* surface, QSurfaceFormat format)
    : m_guiViewer(viewer)
    , m_running(false)
    , m_surface(surface)
    , m_format(format)
{
    m_glContext = new QOpenGLContext(this);
    m_glContext->setShareContext(context);
    m_glContext->setFormat(m_format);
    if (m_glContext->create())
    {
        fprintf(stderr, "Shared context correctly created !\n");
    }
}

EngineThread::~EngineThread()
{
}

void EngineThread::initialize(uint width, uint height)
{
    if (!m_glContext->makeCurrent(m_surface))
    {
        fprintf(stderr, "An error occured :(\n");
    }
//    m_guiViewer->makeCurrent();

    std::cerr << "***Radium Engine Viewer***" << std::endl;
    std::cerr << "Renderer : " << glGetString(GL_RENDERER) << std::endl;
    std::cerr << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
    std::cerr << "OpenGL v." << glGetString(GL_VERSION) << std::endl;
    std::cerr << "GLSL v." << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    ComponentManager* cManager = ComponentManager::createInstance();
    EntityManager* eManager = EntityManager::createInstance();

    m_renderer = new ForwardRenderer(width, height);
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

    m_glContext->doneCurrent();
}

void EngineThread::resize(uint width, uint height)
{
    m_renderer->resize(width, height);
}

void EngineThread::run()
{
    m_glContext->makeCurrent(m_surface);
    while (!isInterruptionRequested())
    {
        static Scalar rotation = 0.0;
        rotation += 0.01;

        Entity* ent = EntityManager::getInstancePtr()->getEntity(0);
        ent->setTransform(Transform(AngleAxis(std::sin(rotation), Vector3(0.0, 1.0, 0.0))));

        m_renderer->update();
        m_glContext->swapBuffers(m_surface);
    }
    m_glContext->doneCurrent();

    fprintf(stderr, "Bye, World !\n");
    emit(finished());
}


void EngineThread::mousePressEvent(QMouseEvent* event)
{
    fprintf(stderr, "mousePressEvent.\n");
}

void EngineThread::mouseReleaseEvent(QMouseEvent* event)
{
    fprintf(stderr, "mouseReleaseEvent.\n");
}

void EngineThread::mouseMoveEvent(QMouseEvent* event)
{
    fprintf(stderr, "mouseMoveEvent.\n");
}

void EngineThread::wheelEvent(QWheelEvent* event)
{
    fprintf(stderr, "wheelEvent.\n");
}

void EngineThread::keyPressEvent(QKeyEvent* event)
{
    fprintf(stderr, "keyPressEvent.\n");
}

void EngineThread::keyReleaseEvent(QKeyEvent* event)
{
    fprintf(stderr, "keyReleaseEvent.\n");
}

} // namespace Ra
