#ifndef RADIUMENGINE_TRACKBALLCAMERA_HPP
#define RADIUMENGINE_TRACKBALLCAMERA_HPP

#include <Engine/Renderer/Camera/CameraInterface.hpp>

namespace Ra { namespace Core { struct MouseEvent; } }
namespace Ra { namespace Core { struct KeyEvent; } }

namespace Ra { namespace Engine {

class TrackballCamera : public CameraInterface
{
public:
    TrackballCamera(uint width, uint height);
    virtual ~TrackballCamera();

    virtual bool handleMouseEvent(Core::MouseEvent* event) override;
    virtual bool handleKeyEvent(Core::KeyEvent* event) override;

    virtual void moveCameraTo(const Core::Vector3& position) override;
    virtual void moveCameraToFitAabb(const Core::Aabb& aabb) override;

private:
    void handleCameraRotate(Scalar dx, Scalar dy);
    void handleCameraPan(Scalar dx, Scalar dy);
    void handleCameraZoom(Scalar dx, Scalar dy);

private:
    Core::Vector3 m_cameraPosition;
    Core::Vector3 m_trackballCenter;

    // Scale manipulation speed given distance to center
    Scalar m_distanceToCenter;
    Scalar m_lastMouseX;
    Scalar m_lastMouseY;

    bool m_cameraRotateMode;
    bool m_cameraPanMode;
    bool m_cameraZoomMode;


};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TRACKBALLCAMERA_HPP