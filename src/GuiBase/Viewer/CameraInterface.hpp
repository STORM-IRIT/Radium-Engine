#ifndef RADIUMENGINE_CAMERAINTERFACE_HPP
#define RADIUMENGINE_CAMERAINTERFACE_HPP
#include <GuiBase/RaGuiBase.hpp>

#include <memory>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>

#include <Core/CoreMacros.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Log.hpp>

#include <GuiBase/Utils/KeyMappingManager.hpp>

namespace Ra {
namespace Engine {
class Camera;
class Light;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Gui {

/// The CameraInterface class is the generic class for camera manipulators.
class RA_GUIBASE_API CameraInterface : public QObject
{
    Q_OBJECT

  public:
    /// Initializes the default app Camera from the given size.
    CameraInterface( uint width, uint height );

    virtual ~CameraInterface();

    /// Resize the camera viewport.
    void resizeViewport( uint width, uint height );

    /// @return the projection matrix.
    Core::Matrix4 getProjMatrix() const;

    /// @return the view matrix.
    Core::Matrix4 getViewMatrix() const;

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMousePressEvent( QMouseEvent* event,
                                        const KeyMappingManager::KeyMappingAction& action ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseReleaseEvent( QMouseEvent* event,
                                          const KeyMappingManager::KeyMappingAction& action ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseMoveEvent( QMouseEvent* event,
                                       const KeyMappingManager::KeyMappingAction& action ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleWheelEvent( QWheelEvent* event,
                                   const KeyMappingManager::KeyMappingAction& action ) = 0;

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyPressEvent( QKeyEvent* event,
                                      const KeyMappingManager::KeyMappingAction& action ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyReleaseEvent( QKeyEvent* event,
                                        const KeyMappingManager::KeyMappingAction& action ) = 0;

    /// Pointer access to the camera.
    const Engine::Camera* getCamera() const { return m_camera; }

    /// Pointer access to the camera.
    Engine::Camera* getCamera() { return m_camera; }

    /// Set the Camera used to render the scene.
    /// \note CameraInterface doesn't have ownership.
    virtual void setCamera( Engine::Camera* camera ) = 0;

    /**
     * Set the Engine::Camera used to the default one.
     * This method allow to have a quick fix of issue #378 before switching to Radium v2
     * development. \todo have a cleaner camera management and control in the GuiBase Radium
     * library. GuiBase Camera interface Must define a clean interface between the application and
     * the Engine. This method is similar to the getCameraFromViewer, it should not be there ...
     */
    void resetToDefaultCamera();

    /// Set the Light attached to the camera.
    /// \note CameraInterface doesn't have ownership.
    void attachLight( Engine::Light* light );

    /// @return true if a Light is attached to the camera, false otherwise.
    bool hasLightAttached() const { return m_light != nullptr; }

    /// pointer acces to the attached light if it exists, returns nullptr otherwise.
    Engine::Light* getLight() { return m_light; }

    /// Static method to get the Camera from the given viewer.
    // FIXME: shouldn't be here!
    static const Engine::Camera& getCameraFromViewer( QObject* v );

  public slots:
    /// \name Camera properties setters
    ///@{
    void setCameraSensitivity( double sensitivity );
    void setCameraFov( double fov );
    void setCameraFovInDegrees( double fov );
    void setCameraZNear( double zNear );
    void setCameraZFar( double zFar );
    ///@}

    /// Set the AABB to restrain the camera behavior against.
    void mapCameraBehaviourToAabb( const Core::Aabb& aabb );

    /// Free the camera from AABB restriction.
    void unmapCameraBehaviourToAabb();

    /// Setup the Camera according to the AABB of the scene to render.
    virtual void fitScene( const Core::Aabb& aabb ) = 0;

    /// Set the Camera position to \p position.
    virtual void setCameraPosition( const Core::Vector3& position ) = 0;

    /// Set the Camera target to \p target.
    virtual void setCameraTarget( const Core::Vector3& target ) = 0;

    /// Reset the Camera settings to default values.
    virtual void resetCamera() = 0;

  signals:
    /// Emitted when the position of the Camera has changed.
    void cameraPositionChanged( const Core::Vector3& );

    /// Emitted when the target of the Camera has changed.
    void cameraTargetChanged( const Core::Vector3& );

    /// Emitted when both the position and the target of the Camera has changed.
    /// \note cameraPositionChanged and cameraTargetChanged are not called in such a case.
    void cameraChanged( const Core::Vector3& position, const Core::Vector3& target );

  protected:
    Scalar m_cameraSensitivity; ///< the Camera sensitivity to manipulation.

    Core::Aabb m_targetedAabb;       ///< Camera behavior restriction AABB.
    Scalar m_targetedAabbVolume;     ///< Volume of the m_targetedAabb
    bool m_mapCameraBahaviourToAabb; ///< whether the camera is restrained or not

    Engine::Camera* m_camera; ///< The Camera.

    Engine::Light* m_light; /// The light attached to the Camera.
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_CAMERAINTERFACE_HPP
