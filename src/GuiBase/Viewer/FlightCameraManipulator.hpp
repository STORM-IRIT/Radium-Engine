#ifndef RADIUMENGINE_FLIGHTCAMERAMANIPULATOR_HPP
#define RADIUMENGINE_FLIGHTCAMERAMANIPULATOR_HPP
#include <GuiBase/RaGuiBase.hpp>

#include <GuiBase/Viewer/CameraManipulator.hpp>

namespace Ra {
namespace Gui {

/// A Flight manipulator for Cameras.
class RA_GUIBASE_API FlightCameraManipulator : public CameraManipulator,
                                               public KeyMappingManageable<FlightCameraManipulator>
{
    Q_OBJECT
    friend class KeyMappingManageable<FlightCameraManipulator>;

  public:
    FlightCameraManipulator( uint width, uint height );
    explicit FlightCameraManipulator( const FlightCameraManipulator& other );
    explicit FlightCameraManipulator( const CameraManipulator& other );
    virtual ~FlightCameraManipulator();

    KeyMappingManager::Context mappingContext();
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
    void initializeFixedUpVector();
    Ra::Core::Vector3 m_fixUpVector{0_ra, 1_ra, 0_ra};
    Scalar m_flightSpeed{1._ra};
    static void configureKeyMapping_impl();

#define KeyMappingFlightManipulator      \
    KMA_VALUE( FLIGHTMODECAMERA_ROTATE ) \
    KMA_VALUE( FLIGHTMODECAMERA_PAN )    \
    KMA_VALUE( FLIGHTMODECAMERA_ZOOM )   \
    KMA_VALUE( FLIGHTMODECAMERA_ROTATE_AROUND )

#define KMA_VALUE( XX ) static KeyMappingManager::KeyMappingAction XX;
    KeyMappingFlightManipulator
#undef KMA_VALUE
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_FLIGHTCAMERAMANIPULATOR_HPP
