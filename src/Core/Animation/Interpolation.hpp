#ifndef RADIUMENGINE_INTERPOLATION_HPP
#define RADIUMENGINE_INTERPOLATION_HPP

#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

inline void interpolate( const Core::Vector3& v0, const Core::Vector3& v1, const Scalar t,
                         Core::Vector3& result ) {
    result = ( ( ( Scalar( 1. ) - t ) * v0 ) + ( t * v1 ) );
}

inline void interpolate( const Core::Quaternion& q0, const Core::Quaternion& q1, const Scalar t,
                         Core::Quaternion& result ) {
    result = q0.slerp( t, q1 );
}

inline void interpolate( const Core::Transform& T0, const Core::Transform& T1, const Scalar t,
                         Core::Transform& result ) {
    Ra::Core::Matrix3 T0R, T1R;
    Ra::Core::Matrix3 T0S, T1S;
    T0.computeRotationScaling( &T0R, &T0S );
    T1.computeRotationScaling( &T1R, &T1S );

    Ra::Core::Quaternion T0Rot = Ra::Core::Quaternion( T0R );
    Ra::Core::Quaternion T1Rot = Ra::Core::Quaternion( T1R );
    Ra::Core::Quaternion iR = T0Rot.slerp( t, T1Rot );
    Ra::Core::Matrix3 iS = ( 1 - t ) * T0S + t * T1S;
    Ra::Core::Vector3 iT = ( 1 - t ) * T0.translation() + t * T1.translation();

    result.fromPositionOrientationScale( iT, iR, iS.diagonal() );
}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INTERPOLATION_HPP
