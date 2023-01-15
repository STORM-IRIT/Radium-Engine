#pragma once
#include <Gui/RaGui.hpp>

#include <Core/Asset/Camera.hpp>
#include <Engine/Scene/CameraComponent.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>

namespace Ra {
namespace Gui {

/// A Flight manipulator for Cameras.
//! [Declare class]
class RA_GUI_API FlightCameraManipulator : public CameraManipulator,
                                           public KeyMappingManageable<FlightCameraManipulator>
{
    Q_OBJECT
    //! [Declare class]
    friend class KeyMappingManageable<FlightCameraManipulator>;

  public:
    using KeyMapping = KeyMappingManageable<FlightCameraManipulator>;

    FlightCameraManipulator();
    explicit FlightCameraManipulator( const CameraManipulator& other );
    virtual ~FlightCameraManipulator();

    KeyMappingManager::Context mappingContext() override;
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

    void updateCamera() override;

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

  private:
    void setupKeyMappingCallbacks();
    void panCallback( QEvent* event );
    void rotateCallback( QEvent* event );
    void zoomCallback( QEvent* event );
    /// x-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseX { 0_ra };

    /// y-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseY { 0_ra };

    /// Whether the corresponding camera movement is active or not.
    bool m_cameraRotateMode { false };
    bool m_cameraPanMode { false };
    bool m_cameraZoomMode { false };

    KeyMappingCallbackManager m_keyMappingCallbackManager;

    void initializeFixedUpVector();
    Ra::Core::Vector3 m_fixUpVector { 0_ra, 1_ra, 0_ra };
    Scalar m_flightSpeed { 1._ra };
    static void configureKeyMapping_impl();

  protected:
#define KeyMappingFlightManipulator      \
    KMA_VALUE( FLIGHTMODECAMERA_PAN )    \
    KMA_VALUE( FLIGHTMODECAMERA_ROTATE ) \
    KMA_VALUE( FLIGHTMODECAMERA_ZOOM )

#define KMA_VALUE( XX ) static KeyMappingManager::KeyMappingAction XX;
    KeyMappingFlightManipulator
#undef KMA_VALUE
};

} // namespace Gui
} // namespace Ra
