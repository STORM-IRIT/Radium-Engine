#include <MainApplication/Viewer/TrackballCamera.hpp>

#include <cmath>
#include <algorithm>

#include <Core/Event/MouseEvent.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{

Gui::TrackballCamera::TrackballCamera(uint width, uint height)
    : CameraInterface(width, height)
    , m_cameraPosition(0, 0, -1)
    , m_trackballCenter(0, 0, 0)
    , m_cameraRotateMode(false)
    , m_cameraPanMode(false)
    , m_cameraZoomMode(false)
{
    m_camera->setTargetPoint(m_trackballCenter);
    m_camera->setPosition(m_cameraPosition, Engine::Camera::ModeType::TARGET);
    m_distanceToCenter = (m_cameraPosition - m_trackballCenter).norm();

    updatePhiTheta();
}

Gui::TrackballCamera::~TrackballCamera()
{
}

void Gui::TrackballCamera::resetCamera()
{
    m_camera->setFrame(Core::Transform::Identity());
    m_camera->setTargetPoint(Core::Vector3(0, 0, 0));
    m_camera->setPosition(Core::Vector3(0, 0, -1), Engine::Camera::ModeType::TARGET);
    m_distanceToCenter = (m_cameraPosition - m_trackballCenter).norm();
    updatePhiTheta();

    m_viewIsDirty = true;
}

bool Gui::TrackballCamera::handleMousePressEvent(QMouseEvent* event)
{
    // Whole manipulation is done with left button and modifiers
    if (event->button() != Qt::LeftButton)
    {
        return false;
    }

    bool handled = false;
    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    if (event->modifiers().testFlag(Qt::NoModifier))
    {
        m_cameraRotateMode = true;
        handled = true;
    }

    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        m_cameraPanMode = true;
        handled = true;
    }

    if (event->modifiers().testFlag(Qt::ShiftModifier))
    {
        m_cameraZoomMode = true;
        handled = true;
    }

    return handled;
}

bool Gui::TrackballCamera::handleMouseMoveEvent(QMouseEvent *event)
{
    Scalar dx = (event->pos().x() - m_lastMouseX) / m_width;
    Scalar dy = (event->pos().y() - m_lastMouseY) / m_height;

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

    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    return true;
}

bool Gui::TrackballCamera::handleMouseReleaseEvent(QMouseEvent *event)
{
    m_cameraRotateMode = false;
    m_cameraPanMode = false;
    m_cameraZoomMode = false;

    return true;
}

bool Gui::TrackballCamera::handleKeyPressEvent(QKeyEvent*)
{
    // No keyboard manipulation
    return false;
}

bool Gui::TrackballCamera::handleKeyReleaseEvent(QKeyEvent*)
{
    return false;
}

void Gui::TrackballCamera::moveCameraTo(const Core::Vector3& position)
{
    m_camera->setPosition(position, Engine::Camera::ModeType::TARGET);
}

void Gui::TrackballCamera::moveCameraToFitAabb(const Core::Aabb& aabb)
{
    CameraInterface::moveCameraToFitAabb(aabb);
    m_distanceToCenter = std::min((m_camera->getPosition() - m_camera->getTargetPoint()).norm(),
                                  Scalar(100.0));
    updatePhiTheta();
}

void Gui::TrackballCamera::handleCameraRotate(Scalar dx, Scalar dy)
{
    Scalar x = dx * m_cameraSensitivity;
    Scalar y = -dy * m_cameraSensitivity;
    constexpr Scalar Pi = M_PI;

    Scalar phi = m_phi + x; // Keep phi between -pi and pi
    Scalar theta = std::min(std::max(m_theta + y, Scalar(0.0)), Pi);

    Scalar dphi = phi - m_phi;
    Scalar dtheta = theta - m_theta;

    const Core::Vector3 C = m_camera->getTargetPoint();
    const Core::Vector3 P0 = m_camera->getPosition();
    const Scalar r = m_distanceToCenter;
    // Compute new camera position, on the sphere of radius r centered on C
    Scalar px = C.x() + r * std::cos(phi) * std::sin(theta);
    Scalar py = C.y() + r * std::cos(theta);
    Scalar pz = C.z() + r * std::sin(phi) * std::sin(theta);

    // Compute the translation from old pos to new pos
    Core::Vector3 P(px, py, pz);
    Core::Vector3 t(P - P0);

    // Translate the camera given this translation
    Core::Transform T(Core::Transform::Identity());
    T.translation() = t;

    // Rotate the camera so that it points to the center
    Core::Transform R1(Core::Transform::Identity());
    Core::Transform R2(Core::Transform::Identity());

    Core::Vector3 U = Core::Vector3(0, 1, 0);
    Core::Vector3 R = m_camera->getRightVector().normalized();

    R1 = Core::AngleAxis(-dphi, U);
    R2 = Core::AngleAxis(-dtheta, R);

    // FIXME(Charly): Transform order
    m_camera->applyTransform(T * R1 * R2);

    m_phi = phi;
    m_theta = theta;
    m_viewIsDirty = true;
}

void Gui::TrackballCamera::handleCameraPan(Scalar dx, Scalar dy)
{
    Scalar x = dx * m_distanceToCenter * m_cameraSensitivity;
    Scalar y = dy * m_distanceToCenter * m_cameraSensitivity;
    // Move camera and trackball center, keep the distance to the center
    Core::Vector3 R = m_camera->getRightVector();
    Core::Vector3 U = m_camera->getUpVector();

    Core::Transform T(Core::Transform::Identity());
    Core::Vector3 t = x * R + y * U;
    T.translate(t);

    m_camera->applyTransform(T, Engine::Camera::ModeType::FREE);

    m_viewIsDirty = true;
}

void Gui::TrackballCamera::handleCameraZoom(Scalar dx, Scalar dy)
{
    Scalar y = dy * m_distanceToCenter * m_cameraSensitivity;
    Core::Vector3 F = m_camera->getDirection();

    Core::Transform T(Core::Transform::Identity());
    Core::Vector3 t = y * F;
    T.translate(t);

    m_camera->applyTransform(T, Engine::Camera::ModeType::TARGET);

    m_distanceToCenter = std::min(Scalar(100.0), (m_camera->getPosition() - m_camera->getTargetPoint()).norm());

    m_viewIsDirty = true;
}

void Gui::TrackballCamera::updatePhiTheta()
{
    const Core::Vector3 P = m_camera->getPosition();
    const Core::Vector3 C = m_camera->getTargetPoint();
    const Scalar r = m_distanceToCenter;

    m_theta = std::acos((P.y() - C.y()) / r);
    m_phi   = std::atan((P.z() - C.z()) / (P.x() - C.x()));
}

} // namespace Ra
