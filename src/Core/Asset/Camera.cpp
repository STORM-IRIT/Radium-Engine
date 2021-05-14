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

    Core::Vector3 d0 = getDirection();
    Core::Vector3 d1 = direction.normalized();

    Core::Vector3 c = d0.cross( d1 );
    Scalar d        = d0.dot( d1 );

    // Special case if two directions are exactly opposites we constrain.
    // to rotate around the up vector.
    if ( c.isApprox( Core::Vector3::Zero() ) && d < 0.0 )
    { T.rotate( Core::AngleAxis( Core::Math::Pi, getUpVector() ) ); }
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
        const Scalar l  = -dx; // left
        const Scalar r  = dx;  // right
        const Scalar t  = dy;  // top
        const Scalar b  = -dy; // bottom
        m_projMatrix    = ortho( l, r, b, t, m_zNear, m_zFar );
    }
    break;

    case ProjType::PERSPECTIVE: {
        m_projMatrix = perspective( m_aspect, m_zoomFactor * m_fov, m_zNear, m_zFar );
    }
    break;

    default:
        break;
    }
}

Core::Matrix4 Camera::perspective( Scalar a, Scalar fov, Scalar n, Scalar f ) {
    Core::Matrix4 projMatrix;
    Scalar tanf = std::tan( fov * .5_ra );

    // clang-format off
    projMatrix << 1_ra / tanf,     0_ra,                  0_ra,                         0_ra,
                         0_ra, a / tanf,                  0_ra,                         0_ra,
                         0_ra,     0_ra, ( f + n ) / ( n - f ), ( 2_ra * f * n ) / ( n - f ),
                         0_ra,     0_ra,                 -1_ra,                         0_ra;
    // clang-format on

    return projMatrix;
}

Core::Matrix4 Camera::frustum( Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f ) {
    Core::Matrix4 projMatrix;
    projMatrix.setZero();
    const Scalar A = ( r + l ) / ( r - l );

    const Scalar B = ( t + b ) / ( t - b );
    const Scalar C = ( f + n ) / ( n - f );
    const Scalar D = ( 2_ra * f * n ) / ( n - f );
    // clang-format off
    projMatrix << 2_ra * n / ( r - l ),                  0_ra,     A, 0_ra,
                                  0_ra,  2_ra * n / ( t - b ),     B, 0_ra,
                                  0_ra,                  0_ra,     C,    D,
                                  0_ra,                  0_ra, -1_ra, 0_ra;
    // clang-format on

    return projMatrix;
}

Core::Matrix4 Camera::ortho( Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f ) {
    Core::Matrix4 projMatrix;
    projMatrix.setZero();

    Core::Vector3 tr( -( r + l ) / ( r - l ), -( t + b ) / ( t - b ), -( ( f + n ) / ( f - n ) ) );

    projMatrix.setIdentity();
    projMatrix.coeffRef( 0, 0 )    = 2_ra / ( r - l );
    projMatrix.coeffRef( 1, 1 )    = 2_ra / ( t - b );
    projMatrix.coeffRef( 2, 2 )    = 2_ra / ( n - f );
    projMatrix.block<3, 1>( 0, 3 ) = tr;

    return projMatrix;
}

void Camera::fitZRange( const Core::Aabb& aabb ) {
#if 1
    const Core::Vector3& position = getPosition();
    Core::Vector3 direction       = -m_frame.affine().block<3, 1>( 0, 2 );

    const Core::Vector3& minAabb = aabb.min();
    const Core::Vector3& maxAabb = aabb.max();
    this->m_zNear = this->m_zFar = direction.dot( minAabb - position );

    auto adaptRange = [position, direction, this]( Scalar x, Scalar y, Scalar z ) {
        Ra::Core::Vector3 corner( x, y, z );
        auto d        = direction.dot( corner - position );
        this->m_zNear = std::min( d, this->m_zNear );
        this->m_zFar  = std::max( d, this->m_zFar );
    };
#else

    const auto position  = Ra::Core::Vector3 {0_ra, 0_ra, 0_ra};
    const auto direction = Ra::Core::Vector3 {0_ra, 0_ra, -1_ra};
    const auto view      = getFrame().inverse();
    const auto& minAabb  = aabb.min();
    const auto& maxAabb  = aabb.max();

    m_zNear = m_zFar = direction.dot( view * minAabb - position );
    auto adaptRange  = [view, position, direction, this]( Scalar x, Scalar y, Scalar z ) {
        auto corner   = Core::Vector3 {x, y, z};
        auto d        = direction.dot( view * corner - position );
        this->m_zNear = std::min( d, this->m_zNear );
        this->m_zFar  = std::max( d, this->m_zFar );
    };
#endif

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

Core::Vector3 Camera::project( const Core::Vector3& p ) const {
    Core::Vector4 point   = Core::Vector4::Ones();
    point.head<3>()       = p;
    Core::Vector4 vpPoint = getProjMatrix() * getViewMatrix() * point;
    vpPoint               = vpPoint / vpPoint.w();
    return Core::Vector3( getWidth() * 0.5_ra * ( 1_ra + vpPoint.x() ),
                          getHeight() * 0.5_ra * ( 1_ra - vpPoint.y() ),
                          vpPoint.z() );
}

Core::Vector3 Camera::unProject( const Core::Vector2& pix ) const {
    return unProject( Vector3 {pix.x(), pix.y(), -getZNear()} );
}

Core::Vector3 Camera::unProject( const Core::Vector3& pix ) const {
    const Scalar localX = ( 2_ra * pix.x() ) / getWidth() - 1_ra;
    // Y is "inverted" (goes downwards)
    const Scalar localY = -( 2_ra * pix.y() ) / getHeight() + 1_ra;

    // Multiply the point in screen space by the inverted projection matrix
    // and then by the inverted view matrix ( = m_frame) to get it in world space.
    // NB : localPoint needs to be a vec4 to be multiplied by the proj matrix.
    const Core::Vector4 localPoint( localX, localY, pix.z(), 1_ra );
    const Core::Vector4 unproj = getProjMatrix().inverse() * localPoint;
    return getFrame() * ( unproj.head<3>() / unproj.w() );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
