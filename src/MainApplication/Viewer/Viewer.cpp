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

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/EntityManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Camera/TrackballCamera.hpp>

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
    , m_interactionState(NONE)
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
		std::cout << "GLEW     : " << glewGetString(GLEW_VERSION) << std::endl;
	    GL_CHECK_ERROR;
	}

#endif

    m_renderer = new Engine::Renderer(width(), height());
    m_renderer->initialize();

	m_camera.reset(new Engine::TrackballCamera(width(), height()));
    
    emit ready(this);
}

void Gui::Viewer::setRadiumEngine(Engine::RadiumEngine* engine)
{
    m_engine = engine;
    m_renderer->setEngine(engine);
}

void Gui::Viewer::paintGL()
{
    // TODO(Charly): Setup data, camera handled there.
    Engine::RenderData data;
    data.projMatrix = m_camera->getProjMatrix();
    data.viewMatrix = m_camera->getViewMatrix();

    m_renderer->render(data);
}

void Gui::Viewer::resizeGL(int width, int height)
{
	m_camera->resizeViewport(width, height);
    m_renderer->resize(width, height);
}

void Gui::Viewer::mousePressEvent(QMouseEvent* event)
{
	switch (event->button())
    {
        case Qt::LeftButton:
        {
            if (m_interactionState != NONE)
            {
                // TODO(Charly): Handle interaction mode.
                break;
            }

			Core::MouseEvent e = mouseEventQtToRadium(event);
			e.event = Core::MouseEventType::RA_MOUSE_PRESSED;
			if (m_camera->handleMouseEvent(&e))
			{
				m_interactionState = CAMERA;
			}

        } break;

        case Qt::RightButton:
        {
            // Check picking
            makeCurrent();
            int clicked = m_renderer->checkPicking(event->x(), height() - event->y());
            fprintf(stderr, "Clicked object %d\n", clicked);
            doneCurrent();
        } break;

        default:
        {
        } break;
    }
}
void Gui::Viewer::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_interactionState == CAMERA)
	{
		Core::MouseEvent e = mouseEventQtToRadium(event);
		e.event = Core::MouseEventType::RA_MOUSE_RELEASED;
		m_camera->handleMouseEvent(&e);

		m_interactionState = NONE;
	}
}

void Gui::Viewer::mouseMoveEvent(QMouseEvent* event)
{
	if (m_interactionState == CAMERA)
	{
		Core::MouseEvent e = mouseEventQtToRadium(event);
		e.event = Core::MouseEventType::RA_MOUSE_MOVED;
		m_camera->handleMouseEvent(&e);
	}
}

void Gui::Viewer::wheelEvent(QWheelEvent* event)
{
    QOpenGLWidget::wheelEvent(event);
    getMainWin(this)->viewerWheelEvent(event);
}

void Gui::Viewer::reloadShaders()
{
    makeCurrent();
    m_renderer->reloadShaders();
    doneCurrent();
}

Core::MouseEvent Gui::Viewer::mouseEventQtToRadium(const QMouseEvent* qtEvent)
{
	Core::MouseEvent raEvent;
	switch (qtEvent->button())
	{
		case Qt::LeftButton:
		{
			raEvent.button = Core::MouseButton::RA_MOUSE_LEFT_BUTTON;
		} break;

		case Qt::MiddleButton:
		{
			raEvent.button = Core::MouseButton::RA_MOUSE_MIDDLE_BUTTON;
		} break;

		case Qt::RightButton:
		{
			raEvent.button = Core::MouseButton::RA_MOUSE_RIGHT_BUTTON;
		} break;

		default:
		{
		} break;
	}

	raEvent.modifier = 0;

	if (qtEvent->modifiers().testFlag(Qt::NoModifier))
	{
		raEvent.modifier = Core::Modifier::RA_EMPTY;
	}

	if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
	{
		raEvent.modifier |= Core::Modifier::RA_CTRL_KEY;
	}

	if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
	{
		raEvent.modifier |= Core::Modifier::RA_SHIFT_KEY;
	}

	if (qtEvent->modifiers().testFlag(Qt::AltModifier))
	{
		raEvent.modifier |= Core::Modifier::RA_ALT_KEY;
	}

	raEvent.absoluteXPosition = qtEvent->x();
	raEvent.absoluteYPosition = qtEvent->y();
	return raEvent;
}

void Gui::Viewer::sceneChanged(const Core::Aabb& aabb)
{
	m_camera->moveCameraToFitAabb(aabb);
}

} // namespace Ra
