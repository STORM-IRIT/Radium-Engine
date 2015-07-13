#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <Engine/Renderer/RenderSystem.hpp>

namespace Ra
{

// FIXME (Charly) : Which way do we want to be able to change renderers ? 
//					Can it be done during runtime ? Must it be at startup ? ...
//					For now, default ForwardRenderer is used.
class Viewer : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	/// CONSTRUCTOR
	explicit Viewer(QWidget* parent = nullptr);
	
	/// DESTRUCTOR
	~Viewer();

protected:
	/// OPENGL
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int width, int height) override;

	/// INTERACTION
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

private:
    std::shared_ptr<RenderSystem> m_renderer;
};

} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
