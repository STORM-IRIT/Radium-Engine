#include <Engine/Renderer/Camera/CameraInterface.hpp>

#include <cmath>

#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{

Engine::CameraInterface::CameraInterface(uint width, uint height)
	: m_width(width)
	, m_height(height)
	, m_projIsDirty(true)
	, m_viewIsDirty(true)
	, m_cameraSensitivity(1.0)
{
	m_camera.reset(new Camera());

	setCameraFovInDegrees(60.0);
	setCameraZNear(0.1);
	setCameraZFar(1000000.0);
}

Engine::CameraInterface::~CameraInterface()
{
}

void Engine::CameraInterface::resizeViewport(uint width, uint height)
{
	m_width = width;
	m_height = height;

	m_projIsDirty = true;
}

Core::Matrix4 Engine::CameraInterface::getProjMatrix() const
{
	if (m_projIsDirty)
	{
		m_camera->updateProjMatrix(m_width, m_height);
		m_projIsDirty = false;
	}

	return m_camera->getProjMatrix();
}

Core::Matrix4 Engine::CameraInterface::getViewMatrix() const
{
	if (m_viewIsDirty)
	{
		m_camera->updateViewMatrix();
		m_viewIsDirty = false;
	}

	return m_camera->getViewMatrix();
}

void Engine::CameraInterface::setCameraSensitivity(Scalar sensitivity)
{
	m_cameraSensitivity = sensitivity;
}

void Engine::CameraInterface::setCameraFov(Scalar fov)
{
	m_camera->setFOV(fov);
	m_projIsDirty = true;
}

void Engine::CameraInterface::setCameraFovInDegrees(Scalar fov)
{
	m_camera->setFOV(fov * M_PI / 180.0);
	m_projIsDirty = true;
}

void Engine::CameraInterface::setCameraZNear(Scalar zNear)
{
	m_camera->setZNear(zNear);
	m_projIsDirty = true;
}

void Engine::CameraInterface::setCameraZFar(Scalar zFar)
{
	m_camera->setZFar(zFar);
	m_projIsDirty = true;
}

void Engine::CameraInterface::moveCameraTo(const Core::Vector3& position)
{
	m_camera->setPosition(position);
	m_viewIsDirty = true;
}

void Engine::CameraInterface::mapCameraBehaviourToAabb(const Core::Aabb& aabb)
{
	m_targetedAabb = aabb;
	m_targetedAabbVolume = aabb.volume();
	m_mapCameraBahaviourToAabb = true;
}

void Engine::CameraInterface::unmapCameraBehaviourToAabb()
{
	m_mapCameraBahaviourToAabb = false;
}

void Engine::CameraInterface::moveCameraToFitAabb(const Core::Aabb& aabb)
{
	Scalar fov = m_camera->getFOV();

	Scalar halfBoxWidth = (aabb.max().x() - aabb.min().x());

	Core::Vector3 boxCenter = aabb.center();

	Scalar distToBox = halfBoxWidth / std::tan(fov);
	Scalar distToCenter = std::abs(aabb.min().z() - aabb.center().z());

	Scalar newZ = boxCenter.z() - distToBox - distToCenter;
	Core::Vector3 newPos = boxCenter;
	newPos.z() = newZ;

	m_camera->setPosition(newPos);
	m_camera->setTargetPoint(boxCenter);

	
	m_viewIsDirty = true;

	// FIXME(Charly): Should we change camera zFar given bbox size ? 
}

}