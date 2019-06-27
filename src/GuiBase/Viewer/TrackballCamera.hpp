#ifndef RADIUMENGINE_TRACKBALLCAMERA_HPP
#define RADIUMENGINE_TRACKBALLCAMERA_HPP
#include <GuiBase/RaGuiBase.hpp>

#include <GuiBase/Viewer/CameraInterface.hpp>

namespace Ra {
namespace Gui {

/// A Trackball manipulator for Cameras.
class RA_GUIBASE_API TrackballCamera : public CameraInterface
{
    Q_OBJECT

  public:
    TrackballCamera( uint width, uint height );
    virtual ~TrackballCamera();

    static void registerKeyMapping();

    bool handleMousePressEvent( QMouseEvent* event,
                                const Qt::MouseButtons& buttons,
                                const Qt::KeyboardModifiers& modifiers,
                                int key ) override;
    bool handleMouseReleaseEvent( QMouseEvent* event ) override;
    bool handleMouseMoveEvent( QMouseEvent* event,
                               const Qt::MouseButtons& buttons,
                               const Qt::KeyboardModifiers& modifiers,
                               int key ) override;
    bool handleWheelEvent( QWheelEvent* event ) override;

    bool handleKeyPressEvent( QKeyEvent* event,
                              const KeyMappingManager::KeyMappingAction& action ) override;
    bool handleKeyReleaseEvent( QKeyEvent* event ) override;

    void toggleRotateAround();
    void setCamera( Engine::Camera* camera ) override;

    /// Set the distance from the camera to the target point.
    /// \note doesn't modify the camera.
    void setTrackballRadius( Scalar rad );

    /// Return the distance from the camera to the target point.
    Scalar getTrackballRadius() const;

    /// Set the trackball center.
    void setTrackballCenter( const Core::Vector3& c );

    /// Return the trackball center.
    /// \note doesn't modify the camera.
    const Core::Vector3& getTrackballCenter() const;

  public slots:
    void setCameraPosition( const Core::Vector3& position ) override;
    void setCameraTarget( const Core::Vector3& target ) override;
    void fitScene( const Core::Aabb& aabb ) override;

    void resetCamera() override;

  protected:
    virtual void handleCameraRotate( Scalar dx, Scalar dy );
    virtual void handleCameraPan( Scalar dx, Scalar dy );
    virtual void handleCameraZoom( Scalar dx, Scalar dy );
    virtual void handleCameraZoom( Scalar z );

    /// Update the polar coordinates of the Camera w.r.t. the trackball center.
    void updatePhiTheta();

  protected:
    /// center of the trackball.
    Core::Vector3 m_trackballCenter;

    /// x-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseX{0_ra};

    /// y-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseY{0_ra};

    /// Additional factor for camera sensitivity.
    Scalar m_quickCameraModifier;

    /// Zoom speed on mouse wheel events.
    Scalar m_wheelSpeedModifier;

    /// Polar coordinates of the Camera w.r.t. the trackball center.
    Scalar m_phi{0_ra};
    Scalar m_theta{0_ra};

    /// The distance from the camera to the trackball center.
    Scalar m_distFromCenter;

    /// Whether the corresponding camera movement is active or not.
    bool m_rotateAround;
    bool m_cameraRotateMode;
    bool m_cameraPanMode;
    bool m_cameraZoomMode;
    // TODO(Charly): fps mode

    // static KeyMappingManager::Context m_keyMappingContext;

#define KeyMappingCamera                      \
    KMA_VALUE( TRACKBALLCAMERA_MANIPULATION ) \
    KMA_VALUE( TRACKBALLCAMERA_ROTATE )       \
    KMA_VALUE( TRACKBALLCAMERA_PAN )          \
    KMA_VALUE( TRACKBALLCAMERA_ZOOM )         \
    KMA_VALUE( TRACKBALLCAMERA_ROTATE_AROUND )

#define KMA_VALUE( XX ) static KeyMappingManager::KeyMappingAction XX;
    KeyMappingCamera
#undef KMA_VALUE
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_TRACKBALLCAMERA_HPP
