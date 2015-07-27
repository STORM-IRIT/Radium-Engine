#include <MainApplication/Viewer/CameraInterface.hpp>

#include <cmath>

#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{

Gui::CameraInterface::CameraInterface(uint width, uint height)
    : m_cameraSensitivity(1.0)
    , m_width(width), m_height(height)
    , m_viewIsDirty(true)
    , m_projIsDirty(true)
{
    m_camera.reset(new Engine::Camera());

    setCameraFovInDegrees(60.0f);
    setCameraZNear(0.1f);
    setCameraZFar(1000000.0f);
}

Gui::CameraInterface::~CameraInterface()
{
}

void Gui::CameraInterface::resizeViewport(uint width, uint height)
{
    m_width = width;
    m_height = height;

    m_projIsDirty = true;
}

Core::Matrix4 Gui::CameraInterface::getProjMatrix() const
{
    if (m_projIsDirty)
    {
        m_camera->updateProjMatrix(m_width, m_height);
        m_projIsDirty = false;
    }

    return m_camera->getProjMatrix();
}

Core::Matrix4 Gui::CameraInterface::getViewMatrix() const
{
    if (m_viewIsDirty)
    {
        m_camera->updateViewMatrix();
        m_viewIsDirty = false;
    }

    return m_camera->getViewMatrix();
}

void Gui::CameraInterface::setCameraSensitivity(double sensitivity)
{
    m_cameraSensitivity = sensitivity;
}

void Gui::CameraInterface::setCameraFov(double fov)
{
    m_camera->setFOV(fov);
    m_projIsDirty = true;
}

void Gui::CameraInterface::setCameraFovInDegrees(double fov)
{
    m_camera->setFOV(fov * M_PI / 180.0);
    m_projIsDirty = true;
}

void Gui::CameraInterface::setCameraZNear(double zNear)
{
    m_camera->setZNear(zNear);
    m_projIsDirty = true;
}

void Gui::CameraInterface::setCameraZFar(double zFar)
{
    m_camera->setZFar(zFar);
    m_projIsDirty = true;
}

void Gui::CameraInterface::mapCameraBehaviourToAabb(const Core::Aabb& aabb)
{
    m_targetedAabb = aabb;
    m_targetedAabbVolume = aabb.volume();
    m_mapCameraBahaviourToAabb = true;
}

void Gui::CameraInterface::unmapCameraBehaviourToAabb()
{
    m_mapCameraBahaviourToAabb = false;
}

void Gui::CameraInterface::moveCameraToFitAabb(const Core::Aabb& aabb)
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
