#ifndef RADIUMENGINE_TRACKBALLCAMERA_HPP
#define RADIUMENGINE_TRACKBALLCAMERA_HPP
#include <GuiBase/RaGuiBase.hpp>

#include <GuiBase/Viewer/CameraInterface.hpp>

namespace Ra {
namespace Gui {

/**
 * A TrackballCamera represents a Camera rotating around a target point.
 */
class RA_GUIBASE_API TrackballCamera : public CameraInterface {
    Q_OBJECT

  public:
    TrackballCamera( uint width, uint height );

    ~TrackballCamera() override;

    bool handleMousePressEvent( QMouseEvent* event ) override;

    bool handleMouseReleaseEvent( QMouseEvent* event ) override;

    bool handleMouseMoveEvent( QMouseEvent* event ) override;

    bool handleWheelEvent( QWheelEvent* event ) override;

    bool handleKeyPressEvent( QKeyEvent* event ) override;

    bool handleKeyReleaseEvent( QKeyEvent* event ) override;

    void setCamera( Engine::Camera* camera ) override;

    /**
     * Set the distance from the camera to the target point.
     * \note Doesn't modify the camera.
     */
    void setTrackballRadius( Scalar rad );

    /**
     * Return the distance from the camera to the target point.
     */
    Scalar getTrackballRadius() const;

    /**
     * Set the trackball center.
     * \note Doesn't modify the camera.
     */
    void setTrackballCenter( const Core::Vector3& c );

    /**
     * Return the trackball center.
     */
    const Core::Vector3& getTrackballCenter() const;

  public slots:
    void setCameraPosition( const Core::Vector3& position ) override;

    void setCameraTarget( const Core::Vector3& target ) override;

    void fitScene( const Core::Aabb& aabb ) override;

    void resetCamera() override;

  protected:
    /**
     * Rotate the camera according to the given pixel difference.
     */
    virtual void handleCameraRotate( Scalar dx, Scalar dy );

    /**
     * Translates the camera according to the given pixel difference.
     * \note Also changes the target point.
     */
    virtual void handleCameraPan( Scalar dx, Scalar dy );

    /**
     * Apply a zoom according to the given pixel difference.
     */
    virtual void handleCameraZoom( Scalar dx, Scalar dy );

    /**
     * Apply a zoom according to the given zoom factor.
     */
    virtual void handleCameraZoom( Scalar z );

    /**
     * Update the polar coordinates of the Camera w.r.t.\ the trackball center.
     */
    void updatePhiTheta();

  protected:
    /// Center of the trackball.
    Core::Vector3 m_trackballCenter;

    /// x-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseX;

    /// y-position of the mouse on the screen at the manipulation start.
    Scalar m_lastMouseY;

    /// Additional factor for camera sensitivity.
    Scalar m_quickCameraModifier;

    /// Zoom speed on mouse wheel events.
    Scalar m_wheelSpeedModifier;

    /// \name Polar coordinates of the Camera w.r.t.\ the trackball center.
    /// \{
    Scalar m_phi;
    Scalar m_theta;
    Scalar m_distFromCenter;
    /// \}

    /** \name Camera Manipulation
     * Whether the corresponding camera movement is active or not.
     */
    /// \{
    bool m_rotateAround;
    bool m_cameraRotateMode;
    bool m_cameraPanMode;
    bool m_cameraZoomMode;
    /// \}
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_TRACKBALLCAMERA_HPP
