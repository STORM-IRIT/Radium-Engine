#pragma once

#include <Core/Asset/Camera.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>
#include <Engine/RaEngine.hpp>
#include <Engine/Scene/Component.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

class Entity;

/// A Camera is an Engine Component storing a Camera object.
class RA_ENGINE_API CameraComponent : public Scene::Component
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    CameraComponent( Entity* entity, const std::string& name, Scalar height, Scalar width );

    ~CameraComponent() override;

    void initialize() override;

    /// Toggle on/off displaying the Camera.
    void show( bool on );

    /// Return the frame of the camera.
    /// Where Y is the up vector and -Z is the direction vector.
    [[deprecated( "Use getCamera()->getFrame() instead" )]] inline Core::Transform getFrame() const;

    /// Set the frame of the camera to 'frame'
    [[deprecated( "Use getCamera()->setFrame() instead" )]] inline void
    setFrame( const Core::Transform& frame );

    /// Return the position.
    [[deprecated( "Use getCamera()->getPosition() instead" )]] inline Core::Vector3
    getPosition() const;

    /// Set the position of the camera to 'position'.
    [[deprecated( "Use getCamera()->setPosition() instead" )]] inline void
    setPosition( const Core::Vector3& position );

    /// Return the direction the camera is looking at.
    [[deprecated( "Use getCamera()->getDirection() instead" )]] inline Core::Vector3
    getDirection() const;

    /// Set the direction of the camera to 'direction'.
    /// The other vectors will be rotated accordingly.
    [[deprecated( "Use getCamera()->setDirection() instead" )]] inline void
    setDirection( const Core::Vector3& direction );

    /// Return the up vector.
    [[deprecated( "Use getCamera()->getUpVector() instead" )]] inline Core::Vector3
    getUpVector() const;

    /// Set the up vector of the camera to 'upVector'.
    /// The other vectors will be rotated accordingly.
    [[deprecated( "Use getCamera()->setUpVector() instead" )]] inline void
    setUpVector( const Core::Vector3& upVector );

    [[deprecated( "Use getCamera()->getRightVector() instead" )]] inline Core::Vector3
    getRightVector() const;

    /// Apply the local transformation 'T' to the camera.
    void applyTransform( const Core::Transform& T );
    void updateTransform();

    //
    // Access to view and projection matrices.
    //

    /// Return the view matrix.
    [[deprecated( "Use getCamera()->getViewMatrix() instead" )]] inline Core::Matrix4
    getViewMatrix() const;

    /// Return the projection matrix.
    [[deprecated( "Use getCamera()->getProjMatrix() instead" )]] inline Core::Matrix4
    getProjMatrix() const;

    /// Update the projection matrix according to the current parameters.
    [[deprecated( "Use getCamera()->updateProjMatrix() instead" )]] void updateProjMatrix();

    //
    // Utility functions
    //

    // Note : in all screen pixels coordinates function, Y is taken to be in standard UI-libs style
    // coordinates, i.e. Y=0 is the top line and Y=height is the bottom line.

    /// Return a ray emanating from the camera, passing by the point given by
    /// screen coordinates x and y.
    [[deprecated( "Use getCamera()->getRayFromScreen() instead" )]] Core::Ray
    getRayFromScreen( const Core::Vector2& pix ) const;

    /// Return the screen coordinates of the given point p (in world coordinates).
    [[deprecated( "Use getCamera()->project() instead" )]] inline Core::Vector2
    project( const Core::Vector3& p ) const;

    /// Return the point on the screen plane (near plane) represented by screen coordinates pix.
    [[deprecated( "Use getCamera()->unProject() instead" )]] inline Core::Vector3
    unProject( const Core::Vector2& pix ) const;

    //
    // Getters and setters for projection matrix parameters.
    //

    /// Return the projection type.
    [[deprecated( "Use getCamera()->getType() instead" )]] inline Ra::Core::Asset::Camera::ProjType
    getType() const;

    /// Set the projection type to 'projectionType'.
    [[deprecated( "Use getCamera()->setType() instead" )]] inline void
    setType( const Ra::Core::Asset::Camera::ProjType& projectionType );

    /// Return the Field Of View.
    /// \note Meaningless for orthogonal projection.
    [[deprecated( "Use getCamera()->getFOV() instead" )]] inline Scalar getFOV() const;

    /// Set the Field Of View to 'fov'.
    /// \note Meaningless for orthogonal projection.
    /// \warning Trigger a rebuild of the projection matrix.
    [[deprecated( "Use getCamera()->setFOV() instead" )]] inline void setFOV( Scalar fov );

    /// Return the zoom factor.
    [[deprecated( "Use getCamera()->getZoomFactor() instead" )]] inline Scalar
    getZoomFactor() const;

    /// Set the zoom factor to 'zoomFactor'.
    [[deprecated( "Use getCamera()->setZoomFactor() instead" )]] inline void
    setZoomFactor( const Scalar& zoomFactor );

    /// \name To be deprecated.
    /// Currently, only the CameraInterface (i.e. TrackballCameraManipulator) calls these
    /// methods. A rework of the rendering architecture will be done soon.
    /// Thus these methods might disappear.
    ///@{
    /// Return the Z Near plane distance from the camera.
    [[deprecated( "Use getCamera()->getZNear() instead" )]] inline Scalar getZNear() const;

    /// Set the Z Near plane distance to 'zNear'.
    [[deprecated( "Use getCamera()->setZNear() instead" )]] inline void setZNear( Scalar zNear );

    /// Return the Z Far plane distance from the camera.
    [[deprecated( "Use getCamera()->getZFar() instead" )]] inline Scalar getZFar() const;

    /// Set the Z Far plane distance to 'zFar'.
    [[deprecated( "Use getCamera()->setZFar() instead" )]] inline void setZFar( Scalar zFar );

    /// Return the width of the viewport.
    [[deprecated( "Use getCamera()->getWidth() instead" )]] inline Scalar getWidth() const;

    /// Return the height of the viewport.
    [[deprecated( "Use getCamera()->getHeight() instead" )]] inline Scalar getHeight() const;

    /// Return the aspect ratio of the viewport.
    [[deprecated( "Use getCamera()->getAspect() instead" )]] inline Scalar getAspect() const;

    /// Change the viewport size.
    [[deprecated( "Use getCamera()->resize() instead" )]] inline void resize( Scalar width,
                                                                              Scalar height );
    ///@}

    /// Duplicate the camera under a different Entity and a different name
    virtual CameraComponent* duplicate( Entity* cloneEntity, const std::string& cloneName ) const;

    /// Fit the Z-range of the camera to the scene's aabb. This will maximize z-buffer precision
    [[deprecated( "Use getCamera()->fitZRange() instead" )]] virtual void
    fitZRange( const Core::Aabb& aabb );

    Core::Asset::Camera* getCamera() const { return m_camera.get(); }
    Core::Asset::Camera* getCamera() { return m_camera.get(); }
    const Rendering::RenderObject* getRenderObject() const { return m_RO; }

  protected:
    std::unique_ptr<Core::Asset::Camera> m_camera;
    Rendering::RenderObject* m_RO {nullptr}; ///< Render mesh for the camera.
};
} // namespace Scene
} // namespace Engine
} // namespace Ra

#include <Engine/Scene/CameraComponent.inl>
