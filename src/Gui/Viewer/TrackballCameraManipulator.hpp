#pragma once
#include <Gui/RaGui.hpp>

#include <Core/Asset/Camera.hpp>
#include <Engine/Scene/CameraComponent.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>

namespace Ra {
namespace Gui {

/// A Trackball manipulator for Cameras.
class RA_GUI_API TrackballCameraManipulator
    : public CameraManipulator,
      public KeyMappingManageable<TrackballCameraManipulator>
{
    Q_OBJECT
    friend class KeyMappingManageable<TrackballCameraManipulator>;

  public:
    using TrackballCameraMapping = KeyMappingManageable<TrackballCameraManipulator>;

    /// Default constructor
    TrackballCameraManipulator();

    /// Copy constructor used when switching camera manipulator
    /// Requires that m_target is on the line of sight of the camera.
    explicit TrackballCameraManipulator( const CameraManipulator& other );

    /// Destructor.
    virtual ~TrackballCameraManipulator();

    // KeyMappingManager::Context mappingContext();

    bool handleMousePressEvent( QMouseEvent* event,
                                const Qt::MouseButtons& buttons,
                                const Qt::KeyboardModifiers& modifiers,
                                int key ) override;
    bool handleMouseReleaseEvent( QMouseEvent* event ) override;
    bool handleMouseMoveEvent( QMouseEvent* event,
                               const Qt::MouseButtons& buttons,
                               const Qt::KeyboardModifiers& modifiers,
                               int key ) override;
    bool handleWheelEvent( QWheelEvent* event,
                           const Qt::MouseButtons& buttons,
                           const Qt::KeyboardModifiers& modifiers,
                           int key ) override;

    bool handleKeyPressEvent( QKeyEvent* event,
                              const KeyMappingManager::KeyMappingAction& action ) override;
    bool handleKeyReleaseEvent( QKeyEvent* event ) override;

    void toggleRotateAround();
    void updateCamera() override;

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
    virtual void handleCameraMoveForward( Scalar dx, Scalar dy );
    virtual void handleCameraMoveForward( Scalar z );

    /// Update the polar coordinates of the Camera w.r.t. the trackball center.
    void updatePhiTheta();

  protected:
    // the center of the trackball is defined by the inherited m_target

    /// x-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseX {0_ra};

    /// y-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseY {0_ra};

    /// Polar coordinates of the Camera w.r.t. the trackball center.
    Scalar m_phi {0_ra};
    Scalar m_theta {0_ra};

    /// The distance from the camera to the trackball center.
    Scalar m_distFromCenter {0_ra};

  private:
    bool checkIntegrity( const std::string& mess ) const;
    static void configureKeyMapping_impl();

  protected:
#define KeyMappingCamera                   \
    KMA_VALUE( TRACKBALLCAMERA_ROTATE )    \
    KMA_VALUE( TRACKBALLCAMERA_PAN )       \
    KMA_VALUE( TRACKBALLCAMERA_ZOOM )      \
    KMA_VALUE( TRACKBALLCAMERA_PROJ_MODE ) \
    KMA_VALUE( TRACKBALLCAMERA_MOVE_FORWARD )

#define KMA_VALUE( XX ) static KeyMappingManager::KeyMappingAction XX;
    KeyMappingCamera
#undef KMA_VALUE
};

} // namespace Gui
} // namespace Ra
