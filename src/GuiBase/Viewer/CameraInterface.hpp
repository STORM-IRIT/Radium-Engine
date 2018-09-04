#ifndef RADIUMENGINE_CAMERAINTERFACE_HPP
#define RADIUMENGINE_CAMERAINTERFACE_HPP

#include <memory>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>

#include <Core/CoreMacros.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
struct MouseEvent;
struct KeyEvent;
} // namespace Core
} // namespace Ra

namespace Ra {
namespace Engine {
class Camera;
class Light;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Gui {

/// The CameraInterface class is responsible to store the camera used for rendering.
class CameraInterface : public QObject {
    Q_OBJECT

  public:
    CameraInterface( uint width, uint height );

    virtual ~CameraInterface();

    /// Resize the camera viewport.
    void resizeViewport( uint width, uint height );

    /// Return the projection Matrix.
    Core::Matrix4 getProjMatrix() const;

    /// Return the view Matrix.
    Core::Matrix4 getViewMatrix() const;

    /// @return true if the event has been taken into account, false otherwise.
    virtual bool handleMousePressEvent( QMouseEvent* event ) = 0;

    /// @return true if the event has been taken into account, false otherwise.
    virtual bool handleMouseReleaseEvent( QMouseEvent* event ) = 0;

    /// @return true if the event has been taken into account, false otherwise.
    virtual bool handleMouseMoveEvent( QMouseEvent* event ) = 0;

    /// @return true if the event has been taken into account, false otherwise.
    virtual bool handleWheelEvent( QWheelEvent* event ) = 0;

    /// @return true if the event has been taken into account, false otherwise.
    virtual bool handleKeyPressEvent( QKeyEvent* event ) = 0;

    /// @return true if the event has been taken into account, false otherwise.
    virtual bool handleKeyReleaseEvent( QKeyEvent* event ) = 0;

    /// Return the Camera.
    const Engine::Camera* getCamera() const { return m_camera.get(); }

    /// Return the Camera.
    Engine::Camera* getCamera() { return m_camera.get(); }

    /// Attach the given light to the camera.
    void attachLight( Engine::Light* light );

    /// @return true if a Light is attached to the camera, false otherwise.
    bool hasLightAttached() const { return m_light != nullptr; }

    /// @return the attached light.
    Engine::Light* getLight() { return m_light; }

    /// Return the camera used for the given Viewer.
    static const Engine::Camera& getCameraFromViewer( QObject* v );

  public slots:
    /// Set the sensitivity of the Camera.
    void setCameraSensitivity( double sensitivity );

    /// Set the Field of View of the Camera.
    void setCameraFov( double fov );

    /// Set the Field of View of the Camera.
    void setCameraFovInDegrees( double fov );

    /// Set the near-plane of the Camera.
    void setCameraZNear( double zNear );

    /// Set the far-plane of the Camera.
    void setCameraZFar( double zFar );

    /// Restrain the Camera behavior to the given AABB.
    void mapCameraBehaviourToAabb( const Core::Aabb& aabb );

    /// Free the Camera from AABB restriction.
    void unmapCameraBehaviourToAabb();

    /// Reset the Camera according to the given AABB target.
    virtual void fitScene( const Core::Aabb& aabb ) = 0;

    /// Set the position of the Camera.
    virtual void setCameraPosition( const Core::Vector3& position ) = 0;

    /// Set the target point of the Camera.
    virtual void setCameraTarget( const Core::Vector3& target ) = 0;

    /// Reset the Camera to the default Camera.
    virtual void resetCamera() = 0;

  signals:
    /// Emitted when the Camera has moved.
    void cameraPositionChanged( const Core::Vector3& );

    /// Emitted when the target of Camera has changed.
    void cameraTargetChanged( const Core::Vector3& );

  protected:
    /// The Camera restriction AABB.
    Core::Aabb m_targetedAabb;

    /// The volume of the Camera restriction AABB.
    Scalar m_targetedAabbVolume;

    /// The sensitivity of the Camera.
    Scalar m_cameraSensitivity;

    /// The Camera.
    std::unique_ptr<Engine::Camera> m_camera;

    /// Whether the Camera is restrained to an AABB.
    bool m_mapCameraBahaviourToAabb;

    /// The Light attached to the Camera.
    Engine::Light* m_light;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_CAMERAINTERFACE_HPP
