#ifndef RADIUMENGINE_INTERPOLATION_HPP
#define RADIUMENGINE_INTERPOLATION_HPP

#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Math {

/// \name Linear Interpolation
/// \{

/**
 * \returns in \p result the linear interpolation between \p v0 and \p v1,
 * according to \p t.
 * \note Not designed for interpolation on integer or boolean types.
 */
template <typename T>
inline T linearInterpolate( const T& v0, const T& v1, const Scalar t ) {
    return ( ( 1_ra - t ) * v0 ) + ( t * v1 );
}

/**
 * \returns in \p result the spherical linear interpolation between \p q0 and \p q1,
 * according to \p t.
 */
template <>
inline Core::Quaternion linearInterpolate<Core::Quaternion>( const Core::Quaternion& q0,
                                                             const Core::Quaternion& q1,
                                                             const Scalar t ) {
    return q0.slerp( t, q1 );
}

/**
 * \returns in \p result the linear interpolation between \p q0 and \p q1,
 * according to \p t. To do so, the rotational parts are interpolated through
 * spherical linear interpolation while the translational parts are
 * interpolated through linear interpolation.
 */
template <>
inline Core::Transform linearInterpolate<Core::Transform>( const Core::Transform& T0,
                                                           const Core::Transform& T1,
                                                           const Scalar t ) {
    Ra::Core::Matrix3 T0R, T1R;
    Ra::Core::Matrix3 T0S, T1S;
    T0.computeRotationScaling( &T0R, &T0S );
    T1.computeRotationScaling( &T1R, &T1S );

    Ra::Core::Quaternion T0Rot = Ra::Core::Quaternion( T0R );
    Ra::Core::Quaternion T1Rot = Ra::Core::Quaternion( T1R );
    Ra::Core::Quaternion iR    = T0Rot.slerp( t, T1Rot );
    Ra::Core::Matrix3 iS       = ( 1 - t ) * T0S + t * T1S;
    Ra::Core::Vector3 iT       = ( 1 - t ) * T0.translation() + t * T1.translation();

    Core::Transform result;
    result.fromPositionOrientationScale( iT, iR, iS.diagonal() );
    return result;
}
/// \}

} // namespace Math
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INTERPOLATION_HPP
