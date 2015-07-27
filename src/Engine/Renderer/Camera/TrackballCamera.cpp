#include <Engine/Renderer/Camera/TrackballCamera.hpp>

#include <Core/Event/MouseEvent.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{

Engine::TrackballCamera::TrackballCamera(uint width, uint height)
	: CameraInterface(width, height)
	, m_cameraRotateMode(false)
	, m_cameraPanMode(false)
	, m_cameraZoomMode(false)
	, m_trackballCenter(0, 0, 0)
	, m_cameraPosition(0, 0, -1)
{
	m_camera->setTargetPoint(m_trackballCenter);
	m_camera->setPosition(m_cameraPosition, Camera::ModeType::TARGET);
	m_distanceToCenter = (m_cameraPosition - m_trackballCenter).norm();
}

Engine::TrackballCamera::~TrackballCamera()
{
}

bool Engine::TrackballCamera::handleMouseEvent(Core::MouseEvent* event)
{
	bool handled = false;

	switch (event->event)
	{
		case Core::MouseEventType::RA_MOUSE_RELEASED:
		{
			m_cameraRotateMode = false;
			m_cameraPanMode = false;
			m_cameraZoomMode = false;

			return true;
		} break;

		case Core::MouseEventType::RA_MOUSE_MOVED:
		{
			Scalar dx = (event->absoluteXPosition - m_lastMouseX) / m_width;
			Scalar dy = (event->absoluteYPosition - m_lastMouseY) / m_height;

			if (m_cameraRotateMode)
			{
				handleCameraRotate(dx, dy);
			}

			if (m_cameraPanMode)
			{
				handleCameraPan(dx, dy);
			}

			if (m_cameraZoomMode)
			{
				handleCameraZoom(dx, dy);
			}

			m_lastMouseX = event->absoluteXPosition;
			m_lastMouseY = event->absoluteYPosition;

			return (m_cameraRotateMode || m_cameraPanMode || m_cameraZoomMode);
		} break;

		case Core::MouseEventType::RA_MOUSE_PRESSED:
		{
			// Whole manipulation is done with left button and modifiers
			if (event->button != Core::MouseButton::RA_MOUSE_LEFT_BUTTON)
			{
				return false;
			}

			if (event->modifier == Core::Modifier::RA_EMPTY)
			{
				m_cameraRotateMode = true;
				handled = true;
			}

			if (event->modifier & Core::Modifier::RA_CTRL_KEY)
			{
				m_cameraPanMode = true;
				handled = true;
			}

			if (event->modifier & Core::Modifier::RA_SHIFT_KEY)
			{
				m_cameraZoomMode = true;
				handled = true;
			}

			m_lastMouseX = event->absoluteXPosition;
			m_lastMouseY = event->absoluteYPosition;

		} break;

		default:
		{
		} break;
	}

	return handled;
}

bool Engine::TrackballCamera::handleKeyEvent(Core::KeyEvent* event)
{
	// No keyboard manipulation
	return false;
}

void Engine::TrackballCamera::moveCameraTo(const Core::Vector3& position)
{
	m_camera->setPosition(position, Camera::ModeType::TARGET);
}

void Engine::TrackballCamera::moveCameraToFitAabb(const Core::Aabb& aabb)
{
	CameraInterface::moveCameraToFitAabb(aabb);
	m_distanceToCenter = std::min(Scalar(100.0), (m_camera->getPosition() - m_camera->getTargetPoint()).norm());
}

void Engine::TrackballCamera::handleCameraRotate(Scalar dx, Scalar dy)
{
	// FIXME(Charly): Do trackball rotation
	Scalar x = -dx * m_cameraSensitivity;
	Scalar y =  dy * m_cameraSensitivity;

	Core::Vector3 R = m_camera->getRightVector();
	Core::Vector3 U = Core::Vector3(0.0, 1.0, 0.0);

	Core::Transform t1(Core::AngleAxis(x, U));
	Core::Transform t2(Core::AngleAxis(y, R));

	m_camera->applyTransform(t2 * t1);

	m_viewIsDirty = true;
}

void Engine::TrackballCamera::handleCameraPan(Scalar dx, Scalar dy)
{
	Scalar x = dx * m_distanceToCenter * m_cameraSensitivity;
	Scalar y = dy * m_distanceToCenter * m_cameraSensitivity;
	// Move camera and trackball center, keep the distance to the center
	Core::Vector3 R = m_camera->getRightVector();
	Core::Vector3 U = m_camera->getUpVector();

	Core::Transform T(Core::Transform::Identity());
	Core::Vector3 t = x * R + y * U;
	T.translate(t);

	m_camera->applyTransform(T, Camera::ModeType::FREE);

	m_viewIsDirty = true;
}

void Engine::TrackballCamera::handleCameraZoom(Scalar dx, Scalar dy)
{
	Scalar y = dy * m_distanceToCenter * m_cameraSensitivity;
	Core::Vector3 F = m_camera->getDirection();

	Core::Transform T(Core::Transform::Identity());
	Core::Vector3 t = y * F;
	T.translate(t);

	m_camera->applyTransform(T, Camera::ModeType::TARGET);

	m_distanceToCenter = std::min(Scalar(100.0), (m_camera->getPosition() - m_camera->getTargetPoint()).norm());

	m_viewIsDirty = true;
}

} // namespace Ra