// FIXME(Charly): Needs to be fixed to be compiled
#ifndef RADIUMENGINE_INTERPOLATION_HPP
#define RADIUMENGINE_INTERPOLATION_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Assets/FileData.hpp>

#ifdef DEBUG_LOAD_ANIMATION
namespace Ra {
namespace Core {

void interpolate( const Core::Vector3& v0, const Core::Vector3& v1, const Scalar t, Core::Vector3 result ) {
    result = ( ( t * v0 ) + ( ( 1.0 - t ) * v1 ) );
}

void interpolate( const Core::Quaternion& q0, const Core::Quaternion& q1, const Scalar t, Core::Quaternion result ) {
    result = q0.slerp( t, q1 );
}

void interpolate( const Core::Transform& T0, const Core::Transform& T1, const Scalar t, Core::Transform result ) {
    Core::Quaternion q0 = Core::Quaternion( T0.rotation() );
    Core::Quaternion q1 = Core::Quaternion( T1.rotation() );
    Core::Quaternion q  = q0.slerp( t, q1 );
    Core::Vector3    tr = ( ( t * F0.translation() ) + ( ( 1.0 - t ) * F1.translation() ) );
    result.linear()      = q.toRotationMatrix();
    result.translation() = tr;
}

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INTERPOLATION_HPP

#endif
