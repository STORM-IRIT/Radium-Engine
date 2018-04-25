#ifndef RADIUMENGINE_INTERPOLATION_HPP
#define RADIUMENGINE_INTERPOLATION_HPP

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Asset {

inline void interpolate( const Core::Math::Vector3& v0, const Core::Math::Vector3& v1, const Scalar t,
                         Core::Math::Vector3& result ) {
    result = ( ( ( 1.0 - t ) * v0 ) + ( t * v1 ) );
}

inline void interpolate( const Core::Math::Quaternion& q0, const Core::Math::Quaternion& q1, const Scalar t,
                         Core::Math::Quaternion& result ) {
    result = q0.slerp( t, q1 );
}

inline void interpolate( const Core::Math::Transform& T0, const Core::Math::Transform& T1, const Scalar t,
                         Core::Math::Transform& result ) {
    Core::Math::Quaternion q;
    Core::Math::Vector3 tr;
    interpolate( Core::Math::Quaternion( T0.rotation() ), Core::Math::Quaternion( T1.rotation() ), t, q );
    interpolate( T0.translation(), T1.translation(), t, tr );
    result.linear() = q.toRotationMatrix();
    result.translation() = tr;
}

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INTERPOLATION_HPP
