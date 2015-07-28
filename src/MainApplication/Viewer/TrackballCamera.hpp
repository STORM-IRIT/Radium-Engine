#ifndef RADIUMENGINE_TRACKBALLCAMERA_HPP
#define RADIUMENGINE_TRACKBALLCAMERA_HPP

#include <MainApplication/Viewer/CameraInterface.hpp>

namespace Ra { namespace Core { struct MouseEvent; } }
namespace Ra { namespace Core { struct KeyEvent; } }

namespace Ra { namespace Gui {

class TrackballCamera : public CameraInterface
{
    Q_OBJECT

public:
    TrackballCamera(uint width, uint height);
    virtual ~TrackballCamera();

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMousePressEvent(QMouseEvent* event) override;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseReleaseEvent(QMouseEvent* event) override;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseMoveEvent(QMouseEvent* event) override;

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyPressEvent(QKeyEvent* event) override;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyReleaseEvent(QKeyEvent* event) override;

public slots:
    virtual void setCameraPosition(const Core::Vector3& position) override;
    virtual void setCameraTarget(const Core::Vector3 &target) override;
    virtual void moveCameraToFitAabb(const Core::Aabb& aabb) override;

    virtual void resetCamera() override;

private:
    void handleCameraRotate(Scalar dx, Scalar dy);
    void handleCameraPan(Scalar dx, Scalar dy);
    void handleCameraZoom(Scalar dx, Scalar dy);

    void updatePhiTheta();

private:
    Core::Vector3 m_cameraPosition;
    Core::Vector3 m_trackballCenter;

    // Scale manipulation speed given distance to center
    Scalar m_distanceToCenter;
    Scalar m_lastMouseX;
    Scalar m_lastMouseY;

    Scalar m_quickCameraModifier;

    Scalar m_phi;
    Scalar m_theta;

    bool m_cameraRotateMode;
    bool m_cameraPanMode;
    bool m_cameraZoomMode;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TRACKBALLCAMERA_HPP
