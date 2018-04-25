#ifndef RADIUMENGINE_CAMERAINTERFACE_HPP
#define RADIUMENGINE_CAMERAINTERFACE_HPP

#include <memory>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>

#include <Core/CoreMacros.hpp>
#include <Core/Utils/Log.hpp>
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

class CameraInterface : public QObject {
    Q_OBJECT

  public:
    // FIXME(Charly): width / height ?
    CameraInterface( uint width, uint height );
    virtual ~CameraInterface();

    void resizeViewport( uint width, uint height );

    Core::Math::Matrix4 getProjMatrix() const;
    Core::Math::Matrix4 getViewMatrix() const;

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMousePressEvent( QMouseEvent* event ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseReleaseEvent( QMouseEvent* event ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseMoveEvent( QMouseEvent* event ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleWheelEvent( QWheelEvent* event ) = 0;

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyPressEvent( QKeyEvent* event ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyReleaseEvent( QKeyEvent* event ) = 0;

    const Engine::Camera* getCamera() const { return m_camera.get(); }

    Engine::Camera* getCamera() { return m_camera.get(); }

    // FIXED (Mathias) Light is a component. Camera doen't have ownership
    void attachLight( Engine::Light* light );
    bool hasLightAttached() const { return m_hasLightAttached; }
    /// pointer acces to the attached light, the caller has to check if
    /// hasLightAttached is true, it return a shared_ptr, so the light
    /// could be attached to another camera
    Engine::Light* getLight() { return m_light; }
    virtual void update( Scalar dt ) {}

    static const Engine::Camera& getCameraFromViewer( QObject* v );

  public slots:
    void setCameraSensitivity( double sensitivity );

    void setCameraFov( double fov );
    void setCameraFovInDegrees( double fov );
    void setCameraZNear( double zNear );
    void setCameraZFar( double zFar );

    void mapCameraBehaviourToAabb( const Core::Math::Aabb& aabb );
    void unmapCameraBehaviourToAabb();

    virtual void fitScene( const Core::Math::Aabb& aabb ) = 0;

    virtual void setCameraPosition( const Core::Math::Vector3& position ) = 0;
    virtual void setCameraTarget( const Core::Math::Vector3& target ) = 0;

    virtual void resetCamera() = 0;

  signals:
    void cameraPositionChanged( const Core::Math::Vector3& );
    void cameraTargetChanged( const Core::Math::Vector3& );

  protected:
    Core::Math::Aabb m_targetedAabb;

    Scalar m_targetedAabbVolume;
    Scalar m_cameraSensitivity;

    std::unique_ptr<Engine::Camera> m_camera;
    bool m_mapCameraBahaviourToAabb;

    Engine::Light* m_light;
    bool m_hasLightAttached;
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_CAMERAINTERFACE_HPP
