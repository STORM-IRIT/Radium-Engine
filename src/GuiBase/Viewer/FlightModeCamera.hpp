#ifndef RADIUMENGINE_FLIGHTMODECAMERA_HPP
#define RADIUMENGINE_FLIGHTMODECAMERA_HPP
#include <GuiBase/RaGuiBase.hpp>

#include <GuiBase/Viewer/CameraInterface.hpp>

namespace Ra {
namespace Gui {

/// A Trackball manipulator for Cameras.
class RA_GUIBASE_API FlightModeCamera : public CameraInterface,
                                        public KeyMappingManageable<FlightModeCamera>
{
    Q_OBJECT
    friend class KeyMappingManageable<FlightModeCamera>;

  public:
    FlightModeCamera( uint width, uint height );
    explicit FlightModeCamera( const CameraInterface* other );
    virtual ~FlightModeCamera();

    KeyMappingManager::Listener mappingConfigurationCallback() override;

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

  protected:
    /// x-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseX{0_ra};

    /// y-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseY{0_ra};

    /// Whether the corresponding camera movement is active or not.
    bool m_rotateAround;
    bool m_cameraRotateMode;
    bool m_cameraPanMode;
    bool m_cameraZoomMode;

  private:
    Scalar m_flightSpeed{1.};
    static void configureKeyMapping_impl();

#define KeyMappingFlightCamera           \
    KMA_VALUE( FLIGHTMODECAMERA_ROTATE ) \
    KMA_VALUE( FLIGHTMODECAMERA_PAN )    \
    KMA_VALUE( FLIGHTMODECAMERA_ZOOM )   \
    KMA_VALUE( FLIGHTMODECAMERA_ROTATE_AROUND )

#define KMA_VALUE( XX ) static KeyMappingManager::KeyMappingAction XX;
    KeyMappingFlightCamera
#undef KMA_VALUE
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_FLIGHTMODECAMERA_HPP
