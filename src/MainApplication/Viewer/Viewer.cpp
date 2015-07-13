#include <MainApplication/Viewer/Viewer.hpp>


#include <iostream>
#include <Engine/Renderer/ForwardRenderer.hpp>

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
    std::cout<<"GLSL v. "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;

    m_renderer = std::make_shared<ForwardRenderer>(width(), height());

}

void Viewer::paintGL()
{
	makeCurrent();
//    glClearColor(1.0, 0.1, 0.1, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT);
	m_renderer->render();
}

void Viewer::resizeGL(int width, int height)
{
	makeCurrent();
	m_renderer->resize(width, height);
}

void Viewer::mousePressEvent(QMouseEvent* event)
{
}

void Viewer::mouseMoveEvent(QMouseEvent* event)
{
}

void Viewer::wheelEvent(QWheelEvent* event)
{
}

void Viewer::keyPressEvent(QKeyEvent* event)
{}

} // namespace Ra
