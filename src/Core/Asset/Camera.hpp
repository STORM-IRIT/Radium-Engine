#pragma once

#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Observable.hpp>
namespace Ra {
namespace Core {
namespace Asset {

class RA_CORE_API Camera
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Define the projection type.
    enum class ProjType { ORTHOGRAPHIC, PERSPECTIVE };

    Camera( Scalar height = 1_ra, Scalar width = 1_ra );

    ~Camera() = default;

    Camera& operator=( const Camera& rhs );

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
    void setDirection( const Core::Vector3& direction );

    /// Return the up vector.
    inline Core::Vector3 getUpVector() const;

    /// Set the up vector of the camera to 'upVector'.
    /// The other vectors will be rotated accordingly.
    inline void setUpVector( const Core::Vector3& upVector );

    inline Core::Vector3 getRightVector() const;

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

    //
    // Getters and setters for projection matrix parameters.
    //

    /// Return the projection type.
    inline ProjType getType() const;

    /// Set the projection type to 'projectionType'.
    inline void setType( const ProjType& projectionType );

    /// Return the zoom factor.
    inline Scalar getZoomFactor() const;

    /// Set the zoom factor to 'zoomFactor'.
    inline void setZoomFactor( const Scalar& zoomFactor );

    /// Return the Field Of View.
    /// \note Meaningless for orthogonal projection.
    inline Scalar getFOV() const;

    /// Set the Field Of View to 'fov'.
    /// \note Meaningless for orthogonal projection.
    /// \warning Trigger a rebuild of the projection matrix.
    inline void setFOV( Scalar fov );

    inline Scalar getMinZNear() const;
    inline Scalar getMinZRange() const;

    /// Return the projection matrix.
    inline Core::Matrix4 getProjMatrix() const;
    inline Core::Matrix4 getViewMatrix() const;

    /// Update the projection matrix according to the current parameters.
    void updateProjMatrix();

    inline void setProjMatrix( Core::Matrix4 projMatrix );

    /// \name To be deprecated.
    /// Currently, only the CameraInterface (i.e. TrackballCameraManipulator) calls these
    /// methods. A rework of the rendering architecture will be done soon.
    /// Thus these methods might disappear.
    ///@{
    /// Return the Z Near plane distance from the camera.
    inline Scalar getZNear() const;

    /// Set the Z Near plane distance to 'zNear'.
    inline void setZNear( Scalar zNear );

    /// Return the Z Far plane distance from the camera.
    inline Scalar getZFar() const;

    /// Set the Z Far plane distance to 'zFar'.
    inline void setZFar( Scalar zFar );

    /// Return the width of the viewport.
    inline Scalar getWidth() const;

    /// Return the height of the viewport.
    inline Scalar getHeight() const;

    /// Return the aspect ratio of the viewport.
    inline Scalar getAspect() const;

    /// Change the viewport size.
    void resize( Scalar width, Scalar height );
    ///@}

    void applyTransform( const Core::Transform& T );

    /// Fit the Z-range of the camera to the scene's aabb. This will maximize z-buffer precision
    void fitZRange( const Core::Aabb& aabb );

    const Scalar m_minZNear {0.01_ra};
    const Scalar m_minZRange {0.01_ra};

  private:
    Core::Transform m_frame {
        Core::Transform::Identity()}; ///< Camera frame (inverse of the view matrix)
    Core::Matrix4 m_projMatrix {Core::Matrix4::Identity()}; ///< Projection matrix

    ProjType m_projType {ProjType::PERSPECTIVE}; ///< Projection type
    Scalar m_zoomFactor {1};                     ///< Zoom factor (modifies the field of view)
    Scalar m_fov {Core::Math::PiDiv4};           ///< Field of view

    /// \name To be deprecated
    /// Currently, only the CameraManipulator (i.e. TrackballCameraManipulator) accesses these
    /// attributes. A rework of the rendering architecture will be done soon.
    /// Thus these attributes might disappear.
    ///@{
    Scalar m_zNear {0.1_ra}; ///< Z Near plane distance
    Scalar m_zFar {1000_ra}; ///< Z Far plane distance
    Scalar m_width {1_ra};   ///< Viewport width (in pixels)
    Scalar m_height {1_ra};  ///< Viewport height (in pixels)
    Scalar m_aspect {1_ra};  ///< Aspect ratio, i.e. width/height. Precomputed for updateProjMatrix.
    ///@}
};
} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/Camera.inl>
