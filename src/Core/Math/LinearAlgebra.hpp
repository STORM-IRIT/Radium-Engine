#ifndef RADIUMENGINE_LINEARALGEBRA_HPP
#define RADIUMENGINE_LINEARALGEBRA_HPP

// This file contains definitions of basic vector functions.

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

/**
 * Print the given matrix to the standard output with à la Matlab formatting.
 */
inline void print( const MatrixN& matrix );

/** \name Vector functions
 * Adaptations for component-wise application, as well as utility functions on vectors.
 */
/// \{

/**
 * Component-wise floor() function on a floating-point vector.
 */
template <typename Vector_>
inline Vector_ floor( const Vector_& v );

/**
 * Component-wise ceil() function on a floating-point vector.
 */
template <typename Vector_>
inline Vector_ ceil( const Vector_& v );

/**
 * Component-wise trunc() function on a floating-point vector.
 */
template <typename Vector_>
inline Vector_ trunc( const Vector_& v );

/**
 * Component-wise clamp() function on a floating-point vector.
 */
template <typename Derived, typename DerivedA, typename DerivedB>
inline typename Derived::PlainMatrix clamp( const Eigen::MatrixBase<Derived>& v,
                                            const Eigen::MatrixBase<DerivedA>& min,
                                            const Eigen::MatrixBase<DerivedB>& max );

/**
 * Component-wise clamp() function on a floating-point vector.
 */
template <typename Derived>
inline typename Derived::PlainMatrix clamp( const Eigen::MatrixBase<Derived>& v, const Scalar& min,
                                            const Scalar& max );

/**
 * Vector range check, works for any numeric vector.
 */
template <typename Vector_>
inline bool checkRange( const Vector_& v, const Scalar& min, const Scalar& max );

/**
 * Get two vectors orthogonal to a given vector.
 * \warning fx must be normalized (this is not checked in the function).
 */
inline void getOrthogonalVectors( const Vector3& fx, Eigen::Ref<Vector3> fy,
                                  Eigen::Ref<Vector3> fz );

/**
 * Get the angle between two vectors. Works for types where the cross product is
 * defined (i.e.\ 2D and 3D vectors).
 */
template <typename Vector_>
inline Scalar angle( const Vector_& v1, const Vector_& v2 );

/**
 * Get the spherical linear interpolation between two unit non-colinear vectors.
 * works for types where the cross-product is defined (i.e.\ 2D and 3D vectors).
 */
template <typename Vector_>
inline Vector_ slerp( const Vector_& v1, const Vector_& v2, Scalar t );

/**
 * Return the projection of point on the plane define by plane and planeNormal.
 */
inline Vector3 projectOnPlane( const Vector3& planePos, const Vector3& planeNormal,
                               const Vector3& point );

/**
 * Get the cotangent of the angle between two vectors. Works for vector types
 * where dot and cross product is defined (2D or 3D vectors).
 */
template <typename Vector_>
inline Scalar cotan( const Vector_& v1, const Vector_& v2 );

/**
 * Get the cosine of the angle between two vectors.
 */
template <typename Vector_>
inline Scalar cos( const Vector_& v1, const Vector_& v2 );

/**
 * Normalize a vector and returns its norm before normalization.
 * If the vector's norm is 0, the vector's components will be overwritten by NaNs.
 */
template <typename Vector_>
inline Scalar getNormAndNormalize( Vector_& v );

/**
 * Normalize a vector and returns its norm before normalization.
 * If the vector's norm is 0, the vector remains null.
 */
template <typename Vector_>
inline Scalar getNormAndNormalizeSafe( Vector_& v );

/**
 * Transform a ray, direction is only transformed by linear part of the
 * transformation, while origin is fully transformed corresponds to t*r.
 * \param t transform matrix.
 * \param r ray to transform.
 * \return the transformed ray, origin is translated while direction is only
 * linarly transformed.
 */
template <typename Scalar>
inline Eigen::ParametrizedLine<Scalar, 3>
transformRay( const Eigen::Transform<Scalar, 3, Eigen::Affine>& t,
              const Eigen::ParametrizedLine<Scalar, 3>& r );
/// \}

/// \name Matrix utils
/// \{

/**
 * Build a look at matrix from the given position, target point and up vector.
 */
inline Matrix4 lookAt( const Vector3& position, const Vector3& target, const Vector3& up );

/**
 * Build a symmetric perspective matrix from the given field of view, aspect ratio,
 * near plane and far plane.
 */
inline Matrix4 perspective( Scalar fovy, Scalar aspect, Scalar near, Scalar zfar );

/**
 * Build an orthographic matrix from the given frustum planes.
 */
inline Matrix4 orthographic( Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar near,
                             Scalar zfar );

/// Call std::isnormal on matrix entry.
/// Dense version
template <typename Matrix_>
inline bool checkInvalidNumbers( Eigen::Ref<const Matrix_> matrix,
                                 const bool FAIL_ON_ASSERT = false );
/// \}

/** \name Quaternion functions
 * Defines functions for multiplying a quaternion by a scalar and adding two quaternions.
 * While Quaternion is supposed to represent a unit quaternion (thus a valid rotation),
 * these functions are useful for linear interpolation of quaternions.
 */
// Note : the .inl file also define operator+ for quaternions
// and operator * and / between quaternions and scalar.
// FIXME: why not declaring these here?
/// \{

/**
 * Returns the quaternion q multipled by a scalar factor of k.
 */
inline Quaternion scale( const Quaternion& q, const Scalar k );

/**
 * Returns the sum of two quaternions.
 */
inline Quaternion add( const Quaternion& q1, const Quaternion& q2 );

/**
 * Returns the sum of two quaternions, resolving antipodality by flipping
 * the sign of q2 if q1.q2 is negative. This operation is usually
 * denoted as a circled + sign, forming the basis of the QLERP algorithm.
 * \see "Spherical Blend Skinning" (Kavan & Zara 2005) for more details.
 */
inline Quaternion addQlerp( const Quaternion& q1, const Quaternion& q2 );

/**
 * Decompose a given rotation \p in into a swing rotation and a twist rotation.
 * \p swingOut is a rotation whose axis lies in the XY plane and \p twistOut is
 * a rotation about axis Z, such that \p in = \p swingOut * \p twistOut.
 * If the rotation is already around axis z, \p swingOut will be set to identity
 * and \p twistOut equal to \p in.
 */
inline void getSwingTwist( const Quaternion& in, Quaternion& swingOut, Quaternion& twistOut );

/**
 * Call std::isnormal on quaternion entries.
 */
template <typename S>
inline bool checkInvalidNumbers( Eigen::Ref<Eigen::Quaternion<S>> q,
                                 const bool FAIL_ON_ASSERT = false ) {
    return checkInvalidNumbers( q.coeffs(), FAIL_ON_ASSERT );
}
/// \}
} // namespace Math
} // namespace Core
} // namespace Ra

#include <Core/Math/LinearAlgebra.inl>

#endif // RADIUMENGINE_LINEARALGEBRA_HPP
