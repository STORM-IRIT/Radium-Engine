#ifndef RADIUMENGINE_CAMERAMANIPULATOR_HPP
#define RADIUMENGINE_CAMERAMANIPULATOR_HPP
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

/// The CameraManipulator class is the generic class for camera manipulators.
class RA_GUIBASE_API CameraManipulator : public QObject
{
    Q_OBJECT

  public:
    /// Initializes a manipulator for a given viewport size.
    CameraManipulator( uint width, uint height );

    /// Initializes a manipulator keeping properties from an already existing one.
    /// This allows to switch from one manipulator to another while keeping the same visual
    /// experience.
    explicit CameraManipulator( const CameraManipulator& other );

    /// Destructor.
    /// As a Manipulator does not have ownership over the associated Camera, do not release the
    /// associated Camera.
    virtual ~CameraManipulator();

    /// Resize the camera viewport.
    void resizeViewport( uint width, uint height );

    /// @return the projection matrix of the manipulated camera.
    Core::Matrix4 getProjMatrix() const;

    /// @return the view matrix of the manipulated camera.
    Core::Matrix4 getViewMatrix() const;

    /// @return the mapping context for keymapping, nullptr if no mapping is available
    virtual KeyMappingManager::Context mappingContext();

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMousePressEvent( QMouseEvent* event,
                                        const Qt::MouseButtons& buttons,
                                        const Qt::KeyboardModifiers& modifiers,
                                        int key ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseReleaseEvent( QMouseEvent* event ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleMouseMoveEvent( QMouseEvent* event,
                                       const Qt::MouseButtons& buttons,
                                       const Qt::KeyboardModifiers& modifiers,
                                       int key ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleWheelEvent( QWheelEvent* event ) = 0;

    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyPressEvent( QKeyEvent* event,
                                      const KeyMappingManager::KeyMappingAction& action ) = 0;
    /// @return true if the event has been taken into account, false otherwise
    virtual bool handleKeyReleaseEvent( QKeyEvent* event ) = 0;

    /// Pointer access to the camera.
    const Engine::Camera* getCamera() const { return m_camera; }

    /// Pointer access to the camera.
    Engine::Camera* getCamera() { return m_camera; }

    /// Set the Camera to be manipulated.
    /// \note CameraManipulator doesn't have ownership.
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
    /// \note CameraManipulator doesn't have ownership.
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
    void setCameraSensitivity( Scalar sensitivity );
    void setCameraFov( Scalar fov );
    void setCameraFovInDegrees( Scalar fov );
    void setCameraZNear( Scalar zNear );
    void setCameraZFar( Scalar zFar );
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
    /// the Camera sensitivity to manipulation.
    Scalar m_cameraSensitivity;
    /// Additional factor for camera sensitivity.
    Scalar m_quickCameraModifier;
    /// Speed modifier on mouse wheel events.
    Scalar m_wheelSpeedModifier;

    Core::Aabb m_targetedAabb;       ///< Camera behavior restriction AABB.
    Scalar m_targetedAabbVolume;     ///< Volume of the m_targetedAabb
    bool m_mapCameraBahaviourToAabb; ///< whether the camera is restrained or not

    /// Target point of the camera (usefull for most of the manipulator metaphor)
    /// Be aware that m_target must always be on the line of sight of the camera so that it could be
    /// used as a "focus" point by a manipulator.
    Core::Vector3 m_target;

    Engine::Camera* m_camera; ///< The Camera.
    Engine::Light* m_light;   ///< The light attached to the Camera.
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_CAMERAMANIPULATOR_HPP
