#pragma once
#include <Core/RaCore.hpp>

#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// \brief Camera class storing the Camera frame and the projection properties
/// The view direction is -z in camera space.
/// \note Both orthonormal and non-orthonormal frames are supported. However, in the current
/// implementation suspect a bug in fitZRange which does not behave as expected for non-orthogonal
/// frames..
class RA_CORE_API Camera
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Define the projection type.
    enum class ProjType { ORTHOGRAPHIC, PERSPECTIVE };

    explicit Camera( Scalar width = 1_ra, Scalar height = 1_ra ) :
        m_width { width }, m_height { height }, m_aspect { width / height } {}

    Camera& operator=( const Camera& rhs );

    /// Return the frame of the camera.
    /// Where Y is the up vector and -Z is the direction vector.
    inline Transform getFrame() const { return m_frame; }

    /// Set the frame of the camera to \b frame.
    inline void setFrame( const Transform& frame ) { m_frame = frame; }

    /// Return the position.
    inline Vector3 getPosition() const { return m_frame.translation(); }

    /// Set the position of the camera to \b position.
    inline void setPosition( const Vector3& position );

    /// Return the direction the camera is looking at.
    inline Vector3 getDirection() const;

    /// Set the direction of the camera to \b direction.
    /// The other vectors will be rotated accordingly.
    void setDirection( const Vector3& direction );

    /// Return the up vector.
    inline Vector3 getUpVector() const;

    /// Set the up vector of the camera to \b upVector.
    /// The other vectors will be rotated accordingly.
    inline void setUpVector( const Vector3& upVector );

    inline Vector3 getRightVector() const;

    //
    // Utility functions
    //

    // Note : in all screen pixels coordinates function, Y is taken to be in standard UI-libs style
    // coordinates, i.e. Y=0 is the top line and Y=height is the bottom line.

    /// Return a ray emanating from the camera, passing by the point given by
    /// screen coordinates x and y.
    Ray getRayFromScreen( const Vector2& pix ) const;

    /// Return the screen coordinates + depth of the given point p (in world coordinates).
    Vector3 projectToScreen( const Vector3& p ) const;

    /// Return NDC cordinate in the view NDC cube \f$ [-1,1]^3 \f$
    Vector3 projectToNDC( const Vector3& p ) const;

    /// Return the 3D point in world space corresponding to screen coordinates pix.
    /// pix.x() and pix.y() are in pixel coordinates \f$ \in [0, getWidth()] \times [0, getHeight()]
    /// \f$. The returned point lies on z near plane.
    Vector3 unProjectFromScreen( const Vector2& pix ) const;

    /// Return the 3D point in world space corresponding to screen pixels pix.x(), pix.y(), at depth
    /// pix.z()
    /// x and y are in pixel coordinates (from (0,0) to width,height, z is in 0, 1 (0 near, 1 far
    /// plane)
    Vector3 unProjectFromScreen( const Vector3& pix ) const;

    /// Return the 3D point in world space corresponding to NDC cube \f$ [-1,1]^3 \f$  point pix.
    Vector3 unProjectFromNDC( const Vector3& pix ) const;

    //
    // Getters and setters for projection matrix parameters.
    //

    /// Return the projection type.
    inline ProjType getType() const { return m_projType; }

    /// Set the projection type to \b projectionType.
    inline void setType( const ProjType& projectionType ) { m_projType = projectionType; }

    /// Return the zoom factor.
    inline Scalar getZoomFactor() const { return m_zoomFactor; }

    /// Set the zoom factor to \b zoomFactor.
    inline void setZoomFactor( const Scalar& zoomFactor );

    /// Return the horizontal Field Of View.
    /// \note Meaningless for orthogonal projection.
    inline Scalar getFOV() const { return m_fov; }

    /// Set the Field Of View to 'fov' in the x (horizontal) direction.
    /// If you have an vertical field of view, you can convert it to horizontal as
    /// Scalar fovx = 2_ra*std::atan( radiumCam->getAspect() * std::tan( cam.yfov / 2_ra ) );
    //        if ( fovxDiv2 < 0_ra ) { fovxDiv2 = Math::PiDiv2; }
    /// \note Meaningless for orthogonal projection.
    /// \warning Trigger a rebuild of the projection matrix.
    inline void setFOV( Scalar fov );

    inline Scalar getMinZNear() const { return m_minZNear; }
    inline Scalar getMinZRange() const { return m_minZRange; }

    /// Return the projection matrix.
    inline Matrix4 getProjMatrix() const { return m_projMatrix; }
    inline Matrix4 getViewMatrix() const { return getFrame().inverse().matrix(); }

    /// Update the projection matrix according to the current parameters.
    void updateProjMatrix();

    /// set projection to \b projMatrix.
    /// \see perspective(), ortho().
    inline void setProjMatrix( Matrix4 projMatrix ) { m_projMatrix = projMatrix; }

    /// Return the Z Near plane distance from the camera.
    inline Scalar getZNear() const { return m_zNear; }

    /// Set the Z Near plane distance to  \b zNear.
    inline void setZNear( Scalar zNear );

    /// Return the Z Far plane distance from the camera.
    inline Scalar getZFar() const { return m_zFar; }

    /// Set the Z Far plane distance to \b zFar.
    inline void setZFar( Scalar zFar );

    /// Return the width of the viewport.
    inline Scalar getWidth() const { return m_width; }

    /// Return the height of the viewport.
    inline Scalar getHeight() const { return m_height; }

    /// Return the aspect ratio of the viewport.
    inline Scalar getAspect() const { return m_aspect; }

    /// Set xmag and ymag for orthographic camera
    inline void setXYmag( Scalar xmag, Scalar ymag );

    /// Get xmag and ymag for orthographic camera
    inline std::pair<Scalar, Scalar> getXYmag() const { return { m_xmag, m_ymag }; }

    /// Change the viewport size. Also compute aspectRatio.
    void setViewport( Scalar width, Scalar height );

    void applyTransform( const Transform& T );

    /// Fit the Z-range of the camera to the scene's aabb. This will maximize z-buffer precision
    void fitZRange( const Aabb& aabb );

    const Scalar m_minZNear { 0.01_ra };
    const Scalar m_minZRange { 0.01_ra };

    /// Compute a project projection matrix as describe here
    /// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#cameras
    /// adapted to horizontal fov
    /// equivalent to scale = tan(m_fov*.5)*m_zNear
    /// frustum(-n*tan(f/2), n*tan(f/2), -n*tan(f/2)/aspect, n*tan(f/2)/aspect, n, f);
    /// \param a : aspect ratio as width/height, i.e. getAspect()
    /// \param y : fov in the horizontal direction, i.e. getFOV()
    /// \param n : z near, i.e. getZNear()
    /// \param f : z far, i.e. getZFar()
    static Matrix4 perspective( Scalar a, Scalar y, Scalar n, Scalar f );

    /// \brief Build a projection matrix from the parameters of the view volume
    /// \see perspective()
    /// Implements the algorithm described here
    /// https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix
    /// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml adapted to our
    /// representation of camera
    /// \param l : left
    /// \param r : right
    /// \param b : bottom
    /// \param t : top
    /// \param n : z near
    /// \param f : z far
    static Matrix4 frustum( Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f );

    /// \brief Build a projection matrix from the parameters of the view volume
    /// Implements the algorithms described here
    /// https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix
    /// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml adapted to our
    /// representation of camera
    /// \param l : left
    /// \param r : right
    /// \param b : bottom
    /// \param t : top
    /// \param n : z near
    /// \param f : z far
    static Matrix4 ortho( Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f );

  private:
    /// Camera frame (inverse of the view matrix).
    /// This represent the transformation from view space to world space.
    Transform m_frame { Transform::Identity() };

    Matrix4 m_projMatrix { Matrix4::Identity() }; ///< Projection matrix

    Scalar m_width { 1_ra };  ///< Viewport width (in pixels)
    Scalar m_height { 1_ra }; ///< Viewport height (in pixels)
    Scalar m_aspect {
        1_ra }; ///< Aspect ratio, i.e. width/height. Precomputed for updateProjMatrix.

    ProjType m_projType { ProjType::PERSPECTIVE }; ///< Projection type
    Scalar m_zoomFactor { 1_ra };                  ///< Zoom factor (modifies the field of view)
    Scalar m_zNear { 0.1_ra };                     ///< Z Near plane distance
    Scalar m_zFar { 1000_ra };                     ///< Z Far plane distance

    /// \name Perspective projection parameters
    ///@{
    Scalar m_fov { Math::PiDiv4 }; ///< Horizontal Field Of View
    ///@}

    /// \name Orthographic projection parameters
    ///@{
    Scalar m_xmag { 1_ra };
    Scalar m_ymag { 1_ra };
    ///@}
};

inline void Camera::setPosition( const Vector3& position ) {
    Transform T     = Transform::Identity();
    T.translation() = position - m_frame.translation();
    applyTransform( T );
}

inline Vector3 Camera::getDirection() const {
    return ( -m_frame.linear().block<3, 1>( 0, 2 ) ).normalized();
}

inline Vector3 Camera::getUpVector() const {
    return m_frame.affine().block<3, 1>( 0, 1 );
}

inline void Camera::setUpVector( const Vector3& upVector ) {
    Transform T = Transform::Identity();
    T.rotate( Quaternion::FromTwoVectors( getUpVector(), upVector ) );
    applyTransform( T );
}

inline Vector3 Camera::getRightVector() const {
    return m_frame.affine().block<3, 1>( 0, 0 );
}

inline void Camera::setFOV( Scalar fov ) {
    m_fov = fov;
    // update m_xmag and m_ymag according to the perspective parameters (heuristic)
    m_xmag = 2_ra * std::tan( m_fov / 2_ra );
    m_ymag = m_xmag;
    updateProjMatrix();
}

inline void Camera::setZNear( Scalar zNear ) {
    m_zNear = zNear;
    updateProjMatrix();
}

inline void Camera::setZFar( Scalar zFar ) {
    m_zFar = zFar;
    updateProjMatrix();
}

inline void Camera::setZoomFactor( const Scalar& zoomFactor ) {
    m_zoomFactor = zoomFactor;
    updateProjMatrix();
}

inline void Camera::setXYmag( Scalar xmag, Scalar ymag ) {
    m_xmag = xmag;
    m_ymag = ymag;
    // update m_fov according to  orthographic parameters (heuristic)
    m_fov = std::atan2( m_xmag * 2, 1_ra );
    updateProjMatrix();
}

} // namespace Asset
} // namespace Core
} // namespace Ra
