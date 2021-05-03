#include <Core/Asset/Camera.hpp>
#include <Engine/Scene/CameraComponent.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Math/Math.hpp>

namespace Ra {

using Core::Math::Pi;
using Core::Math::PiDiv2;
using Core::Math::PiDiv4;

namespace Core {

namespace Asset {

Camera::Camera( Scalar width, Scalar height ) :
    m_width {width}, m_height {height}, m_aspect {width / height} {}

Camera& Camera::operator=( const Camera& rhs ) {
    m_frame      = rhs.getFrame();
    m_projMatrix = rhs.getProjMatrix();
    m_projType   = rhs.getType();
    m_zoomFactor = rhs.getZoomFactor();
    m_fov        = rhs.getFOV();
    m_zNear      = rhs.getZNear();
    m_zFar       = rhs.getZFar();
    setViewport( rhs.getWidth(), rhs.getHeight() );
    return *this;
}

void Camera::setDirection( const Core::Vector3& direction ) {
    Core::Transform T = Core::Transform::Identity();

    auto d0 = getDirection();
    auto d1 = direction.normalized();

    auto c = d0.cross( d1 );
    auto d = d0.dot( d1 );

    // Special case if two directions are exactly opposites we constrain.
    // to rotate around the up vector.
    if ( c.isApprox( Core::Vector3::Zero() ) && d < 0.0 )
    { T.rotate( Core::AngleAxis( Core::Math::PiDiv2, Core::Vector3::UnitY() ) ); }
    else
    { T.rotate( Core::Quaternion::FromTwoVectors( d0, d1 ) ); }
    applyTransform( T );
}

void Camera::setViewport( Scalar width, Scalar height ) {
    m_width  = width;
    m_height = height;
    m_aspect = width / height;
    updateProjMatrix();
}

void Camera::applyTransform( const Core::Transform& T ) {
    Core::Transform t1 = Core::Transform::Identity();
    Core::Transform t2 = Core::Transform::Identity();
    t1.translation()   = -getPosition();
    t2.translation()   = getPosition();

    m_frame = t2 * T * t1 * m_frame;
}

void Camera::updateProjMatrix() {

    switch ( m_projType )
    {
    case ProjType::ORTHOGRAPHIC: {
        const Scalar dx = m_zoomFactor * .5_ra;
        const Scalar dy = dx / m_aspect;
        // ------------
        // Compute projection matrix as describe in the doc of glOrtho()
        const Scalar l = -dx; // left
        const Scalar r = dx;  // right
        const Scalar t = dy;  // top
        const Scalar b = -dy; // bottom
        m_projMatrix   = ortho( b, t, l, r, m_zNear, m_zFar );
    }
    break;

    case ProjType::PERSPECTIVE: {
        // Compute projection matrix as describe here
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix
        auto scale = std::tan( m_fov * .5_ra ) * m_zNear;

        auto r = scale;
        auto l = -r;
        auto t = scale / m_aspect;
        auto b = -t;

        m_projMatrix = frustum( b, t, l, r, m_zNear, m_zFar );
    }
    break;

    default:
        break;
    }
}

Core::Matrix4 Camera::frustum( Scalar b, Scalar t, Scalar l, Scalar r, Scalar n, Scalar f ) {
    Core::Matrix4 projMatrix;
    projMatrix.setZero();
    const Scalar diff           = ( n - f );
    projMatrix.coeffRef( 0, 0 ) = 2_ra * n / ( r - l );
    projMatrix.coeffRef( 1, 1 ) = 2_ra * n / ( t - b );
    projMatrix.coeffRef( 2, 2 ) = ( f + n ) / diff;
    projMatrix.coeffRef( 2, 3 ) = ( 2_ra * f * n ) / diff;
    projMatrix.coeffRef( 3, 2 ) = -1_ra;
    return projMatrix;
}

Core::Matrix4 Camera::ortho( Scalar b, Scalar t, Scalar l, Scalar r, Scalar n, Scalar f ) {
    Core::Matrix4 projMatrix;
    projMatrix.setZero();

    Core::Vector3 tr( -( r + l ) / ( r - l ), -( t + b ) / ( t - b ), -( ( f + n ) / ( f - n ) ) );

    projMatrix.setIdentity();
    projMatrix.coeffRef( 0, 0 )    = 2_ra / ( r - l );
    projMatrix.coeffRef( 1, 1 )    = 2_ra / ( t - b );
    projMatrix.coeffRef( 2, 2 )    = -2_ra / ( f - n );
    projMatrix.block<3, 1>( 0, 3 ) = tr;

    return projMatrix;
}

void Camera::fitZRange( const Core::Aabb& aabb ) {
    const auto& position        = m_frame.translation();
    Ra::Core::Vector3 direction = m_frame.linear() * Ra::Core::Vector3( 0_ra, 0_ra, -1_ra );

    const auto& minAabb = aabb.min();
    const auto& maxAabb = aabb.max();
    m_zNear = m_zFar = direction.dot( minAabb - position );

    auto adaptRange = [position, direction, this]( Scalar x, Scalar y, Scalar z ) {
        Ra::Core::Vector3 corner( x, y, z );
        auto d        = direction.dot( corner - position );
        this->m_zNear = std::min( d, this->m_zNear );
        this->m_zFar  = std::max( d, this->m_zFar );
    };

    adaptRange( minAabb[0], minAabb[1], maxAabb[2] );
    adaptRange( minAabb[0], maxAabb[1], minAabb[2] );
    adaptRange( minAabb[0], maxAabb[1], maxAabb[2] );
    adaptRange( maxAabb[0], maxAabb[1], maxAabb[2] );
    adaptRange( maxAabb[0], maxAabb[1], minAabb[2] );
    adaptRange( maxAabb[0], minAabb[1], maxAabb[2] );
    adaptRange( maxAabb[0], minAabb[1], minAabb[2] );

    if ( m_zNear < m_minZNear ) { m_zNear = m_minZNear; }
    if ( ( m_zFar - m_zNear ) < m_minZRange ) { m_zFar = m_zNear + m_minZRange; }

    updateProjMatrix();
}

Core::Ray Camera::getRayFromScreen( const Core::Vector2& pix ) const {
    // Ray starts from the camera's current position.
    return Core::Ray::Through( getPosition(), unProject( pix ) );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
