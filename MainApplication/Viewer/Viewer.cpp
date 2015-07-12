#include <Viewer/Viewer.hpp>

#include <Renderer/ForwardRenderer.hpp>

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
	m_renderer = std::make_shared<ForwardRenderer>(width(), height());
}

void Viewer::paintGL()
{
	makeCurrent();
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