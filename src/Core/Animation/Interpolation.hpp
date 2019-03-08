#ifndef RADIUMENGINE_INTERPOLATION_HPP
#define RADIUMENGINE_INTERPOLATION_HPP

#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/// \name Interpolation
/// \{

/**
 * Return in \p result the linear interpolation between \p v0 and \p v1,
 * according to \p t.
 */
RA_CORE_API inline void interpolate( const Core::Vector3& v0, const Core::Vector3& v1,
                                     const Scalar t, Core::Vector3& result ) {
    result = ( ( ( Scalar( 1. ) - t ) * v0 ) + ( t * v1 ) );
}

/**
 * Return in \p result the spherical linear interpolation between \p q0 and \p q1,
 * according to \p t.
 */
RA_CORE_API inline void interpolate( const Core::Quaternion& q0, const Core::Quaternion& q1,
                                     const Scalar t, Core::Quaternion& result ) {
    result = q0.slerp( t, q1 );
}

/**
 * Return in \p result the linear interpolation between \p q0 and \p q1,
 * according to \p t. To do so, the rotational parts are interpolated through
 * spherical linear interpolation while the translational parts are
 * interpolated through linear interpolation.
 */
RA_CORE_API inline void interpolate( const Core::Transform& T0, const Core::Transform& T1,
                                     const Scalar t, Core::Transform& result ) {
    Core::Quaternion q;
    Core::Vector3 tr;
    interpolate( Core::Quaternion( T0.rotation() ), Core::Quaternion( T1.rotation() ), t, q );
    interpolate( T0.translation(), T1.translation(), t, tr );
    result.linear() = q.toRotationMatrix();
    result.translation() = tr;
}
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INTERPOLATION_HPP
