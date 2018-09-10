#ifndef RADIUMENGINE_TRACKBALLCAMERA_HPP
#define RADIUMENGINE_TRACKBALLCAMERA_HPP

#include <GuiBase/Viewer/CameraInterface.hpp>

namespace Ra {
namespace Gui {
/// A TrackballCamera represents a Camera rotating around a target point.
class TrackballCamera : public CameraInterface {
    Q_OBJECT

  public:
    TrackballCamera( uint width, uint height );

    virtual ~TrackballCamera();

    bool handleMousePressEvent( QMouseEvent* event ) override;

    bool handleMouseReleaseEvent( QMouseEvent* event ) override;

    bool handleMouseMoveEvent( QMouseEvent* event ) override;

    bool handleWheelEvent( QWheelEvent* event ) override;

    bool handleKeyPressEvent( QKeyEvent* event ) override;

    bool handleKeyReleaseEvent( QKeyEvent* event ) override;

    /// Set the radius of the sphere the camera moves on.
    void setCameraRadius( Scalar rad );

    /// Return the radius of the sphere the camera moves on.
    Scalar getCameraRadius();

  public slots:
    void setCameraPosition( const Core::Vector3& position ) override;

    void setCameraTarget( const Core::Vector3& target ) override;

    void fitScene( const Core::Aabb& aabb ) override;

    void resetCamera() override;

  protected:
    /// Rotate the camera according to the given pixel difference.
    virtual void handleCameraRotate( Scalar dx, Scalar dy );

    /// Translates the camera according to the given pixel difference.
    /// \note Also changes the target point.
    virtual void handleCameraPan( Scalar dx, Scalar dy );

    /// Apply a zoom according to the given pixel difference.
    virtual void handleCameraZoom( Scalar dx, Scalar dy );

    /// Apply a zoom according to the given zoom factor.
    virtual void handleCameraZoom( Scalar z );

    /// Update internal data after moving the camera.
    void updatePhiTheta();

  protected:
    Core::Vector3 m_trackballCenter; ///< The target point.

    Scalar m_lastMouseX; ///< The mouse x-coordinate when grabbing the Camera.
    Scalar m_lastMouseY; ///< The mouse y-coordinate when grabbing the Camera.

    Scalar m_quickCameraModifier; ///< The speed for Camera movements.
    Scalar m_wheelSpeedModifier;  ///< The speed for Camera zoom.

    Scalar m_phi;   ///< The first spherical coordinate of the Camera.
    Scalar m_theta; ///< The second spherical coordinate of the Camera.

    Scalar m_distFromCenter; ///< The screen-space distance from the Camera to the target point.
    Scalar m_cameraRadius;   ///< The world-space distance from Camera to the target point.

    bool m_rotateAround;     ///< Whether the Camera is rotating around the target point or itself.
    bool m_cameraRotateMode; ///< Whether the Camera is rotating.
    bool m_cameraPanMode;    ///< Whether the Camera is translating.
    bool m_cameraZoomMode;   ///< Whether the Camera is zooming.
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_TRACKBALLCAMERA_HPP
