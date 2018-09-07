#ifndef RADIUMENGINE_CAMERA_HPP
#define RADIUMENGINE_CAMERA_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Ray.hpp>

#include <Engine/Component/Component.hpp>

namespace Ra {
namespace Engine {

// FIXME (Hugo) To me this class could totally be renamed LightComponent and get a Light struct
// embedded. Thoughts are welcome !
/// A Light is an Engine Component storing a light object.
class RA_ENGINE_API Camera : public Component {
  public:
    /// Define the projection type.
    enum class ProjType { ORTHOGRAPHIC, PERSPECTIVE };

    RA_CORE_ALIGNED_NEW

    Camera( Entity* entity, const std::string& name, Scalar height, Scalar width );

    ~Camera();

    void initialize() override;

    /// Toggle on/off displaying the Camera.
    void show( bool on );

    /// Return the frame of the camera.
    /// Where Y is the up vector and -Z is the direction vector.
    inline Core::Transform getFrame() const;

    /// Set the frame of the camera to 'frame'
    inline void setFrame( const Core::Transform& frame );

    /// Return the position.
    inline Core::Vector3 getPosition() const;

    /// Set the position of the camera to 'position'.
    inline void setPosition( const Core::Vector3& position );

    /// Return the direction the camera is looking at.
    inline Core::Vector3 getDirection() const;

    /// Set the direction of the camera to 'direction'.
    /// The other vectors will be rotated accordingly.
    inline void setDirection( const Core::Vector3& direction );

    /// Return the up vector.
    inline Core::Vector3 getUpVector() const;

    /// Set the up vector of the camera to 'upVector'.
    /// The other vectors will be rotated accordingly.
    inline void setUpVector( const Core::Vector3& upVector );

    inline Core::Vector3 getRightVector() const;

    /// Apply the local transformation 'T' to the camera.
    void applyTransform( const Core::Transform& T );

    void updateTransform();

    //
    // Getters and setters for projection matrix parameters.
    //

    // Note : any of these functions will trigger a rebuild of the projection matrix

    /// Return the Field Of View.
    inline Scalar getFOV() const;

    /// Set the Field Of View to 'fov'.
    inline void setFOV( const Scalar fov );

    /// Return the Z Near plane distance from the camera.
    inline Scalar getZNear() const;

    /// Set the Z Near plane distance to 'zNear'.
    inline void setZNear( const Scalar zNear );

    /// Return the Z Far plane distance from the camera.
    inline Scalar getZFar() const;

    /// Set the Z Far plane distance to 'zFar'.
    inline void setZFar( const Scalar zFar );

    /// Return the zoom factor.
    inline Scalar getZoomFactor() const;

    /// Set the zoom factor to 'zoomFactor'.
    inline void setZoomFactor( const Scalar& zoomFactor );

    /// Return the projection type.
    inline ProjType getType() const;

    /// Set the projection type to 'projectionType'.
    inline void setType( const ProjType& projectionType );

    /// Return the dimensions of the viewport.
    inline Scalar getWidth() const;
    inline Scalar getHeight() const;
    inline Scalar getAspect() const;

    /// Change the viewport size.
    inline void resize( Scalar width, Scalar height );

    //
    // Access to view and projection matrices.
    //

    /// Return the view matrix.
    inline Core::Matrix4 getViewMatrix() const;

    /// Return the projection matrix.
    inline Core::Matrix4 getProjMatrix() const;

    /// Update the projection matrix according to the current parameters.
    void updateProjMatrix();

    //
    // Utility functions
    //

    // Note : in all screen pixels coordinates function, Y is taken to be in standard UI-libs style
    // coordinates, i.e. Y=0 is the top line and Y=height is the bottom line.

    /// Return a ray emanating from the camera, passing by the point given by
    /// screen coordinates x and y.
    Core::Ray getRayFromScreen( const Core::Vector2& pix ) const;

    /// Return the screen coordinates of the given point p (in world coordinates).
    inline Core::Vector2 project( const Core::Vector3& p ) const;

    /// Return the point on the screen plane (near plane) represented by screen coordinates pix.
    inline Core::Vector3 unProject( const Core::Vector2& pix ) const;

  protected:
    Core::Transform m_frame;    // Camera frame (inverse of the view matrix)
    Core::Matrix4 m_projMatrix; // Projection matrix

    Scalar m_fov;   // Field of view
    Scalar m_zNear; // Z Near plane distance
    Scalar m_zFar;  // Z Far plane distance

    Scalar m_zoomFactor; // Zoom factor (modifies the field of view)

    Scalar m_width;  // Viewport width (in pixels)
    Scalar m_height; // Viewport height (in pixels)
    Scalar m_aspect; // FIXME: never used.

    ProjType m_projType; // Projection type

    RenderObject* m_RO; /// Render mesh for the camera.
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Camera/Camera.inl>

#endif // RADIUMENGINE_CAMERA_HPP
