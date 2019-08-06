#ifndef RADIUMENGINE_LINEARALGEBRA_HPP
#define RADIUMENGINE_LINEARALGEBRA_HPP

/// This file contains definitions of aliases for basic vector classes and functions

#include <Core/RaCore.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>
#include <functional>
#include <unsupported/Eigen/AlignedVector3>

#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Math {
//
// Common vector types
//

inline void print( const MatrixN& matrix );

//
// Geometry types
//

// Todo : storage transform using quaternions ?

//
// Vector Functions
//

/// Component-wise floor() function on a floating-point vector.
template <typename Vector>
inline Vector floor( const Vector& v );

/// Component-wise ceil() function on a floating-point vector.
template <typename Vector>
inline Vector ceil( const Vector& v );

/// Component-wise trunc() function on a floating-point vector
template <typename Vector>
inline Vector trunc( const Vector& v );

/// Component-wise clamp() function on a floating-point vector.

template <typename Derived, typename DerivedA, typename DerivedB>
inline typename Derived::PlainMatrix clamp( const Eigen::MatrixBase<Derived>& v,
                                            const Eigen::MatrixBase<DerivedA>& min,
                                            const Eigen::MatrixBase<DerivedB>& max );
/// Component-wise clamp() function on a floating-point vector.
template <typename Derived>
inline typename Derived::PlainMatrix
clamp( const Eigen::MatrixBase<Derived>& v, const Scalar& min, const Scalar& max );

/// Call std::isnormal on quaternion entries.
template <typename S>
inline bool checkInvalidNumbers( Eigen::Ref<Eigen::Quaternion<S>> q,
                                 const bool FAIL_ON_ASSERT = false ) {
    return checkInvalidNumbers( q.coeffs(), FAIL_ON_ASSERT );
}

/// Call std::isnormal on matrix entry.
/// Dense version
template <typename Matrix_>
inline bool checkInvalidNumbers( Eigen::Ref<const Matrix_> matrix,
                                 const bool FAIL_ON_ASSERT = false );

/// Get two vectors orthogonal to a given vector.
/// \warning fx must be normalized (this is not checked in the function)
inline void
getOrthogonalVectors( const Vector3& fx, Eigen::Ref<Vector3> fy, Eigen::Ref<Vector3> fz );

/// Get the angle between two vectors. Works for types where the cross product is
/// defined (i.e. 2D and 3D vectors).
template <typename Vector_>
inline Scalar angle( const Vector_& v1, const Vector_& v2 );

/// Get the spherical linear interpolation between two unit non-colinear vectors.
/// works for types where the cross-product is defined (i.e. 2D and 3D vectors).
template <typename Vector_>
inline Vector_ slerp( const Vector_& v1, const Vector_& v2, Scalar t );

/// @return the projection of point on the plane define by plane and planeNormal
inline Vector3
projectOnPlane( const Vector3& planePos, const Vector3& planeNormal, const Vector3& point );

/// Get the cotangent of the angle between two vectors. Works for vector types where
/// dot and cross product is defined (2D or 3D vectors).
template <typename Vector_>
inline Scalar cotan( const Vector_& v1, const Vector_& v2 );

/// Get the cosine of the angle between two vectors.
/// \todo use dot instead
template <typename Vector_>
inline Scalar cos( const Vector_& v1, const Vector_& v2 );

/// Normalize a vector and returns its norm before normalization.
/// If the vector's norm is 0, the vector's components will be overwritten by NaNs
template <typename Vector_>
inline Scalar getNormAndNormalize( Vector_& v );

/// Normalize a vector and returns its norm before normalization.
/// If the vector's norm is 0, the vector remains null
template <typename Vector_>
inline Scalar getNormAndNormalizeSafe( Vector_& v );

/// Transform a ray, direction is only transformed by linear part of the
/// transformation, while origin is fully transformed
/// corresponds to t*r
/// \param t : transform matrix
/// \param r : ray to transform
/// \return transoformed ray, origine is translated while direction is only
/// linarly transformed.
template <typename Scalar>
inline Eigen::ParametrizedLine<Scalar, 3>
transformRay( const Eigen::Transform<Scalar, 3, Eigen::Affine>& t,
              const Eigen::ParametrizedLine<Scalar, 3>& r );

inline Matrix4 lookAt( const Vector3& position, const Vector3& target, const Vector3& up );
inline Matrix4 perspective( Scalar fovy, Scalar aspect, Scalar near, Scalar zfar );
inline Matrix4
orthographic( Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar near, Scalar zfar );

//
// Quaternion functions
//

// Define functions for multiplying a quaternion by a scalar
// and adding two quaternions. While Quaternion is supposed to
// represent a unit quaternion (thus a valid rotation), these functions
// are useful for linear interpolation of quaternions.

/// Returns the quaternion q multipled by a scalar factor of k.
inline Quaternion scale( const Quaternion& q, const Scalar k );

/// Returns the sum of two quaternions.
inline Quaternion add( const Quaternion& q1, const Quaternion& q2 );

/// Returns the sum of two quaternions, resolving antipodality by flipping
/// the sign of q2 if q1.q2 is negative. This operation is usually
/// denoted as a circled + sign, forming the basis of the QLERP algorithm.
/// See "Spherical Blend Skinning" (Kavan & Zara 2005) for more details.
inline Quaternion addQlerp( const Quaternion& q1, const Quaternion& q2 );

// Note : the .inl file also define operator+ for quaternions
// and operator * and / between quaternions and scalar.

/// Decompose a given rotation Qin into a swing rotation and a twist rotation.
/// Qswing is a rotation whose axis lies in the XY plane and Qtwist is a rotation about axis Z.
/// such as Qin = Qswing * Qtwist
/// If the rotation is already around axis z, Qswing will be set to identity
/// and Qtwist equal to Qin
inline void getSwingTwist( const Quaternion& in, Quaternion& swingOut, Quaternion& twistOut );

} // namespace Math
} // namespace Core
} // namespace Ra
#include <Core/Math/LinearAlgebra.inl>

#endif // RADIUMENGINE_LINEARALGEBRA_HPP
