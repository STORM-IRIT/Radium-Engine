#ifndef RADIUMENGINE_VECTOR_HPP
#define RADIUMENGINE_VECTOR_HPP

/// This file contains definitions of aliases for basic vector classes and functions

#include <Core/RaCore.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>
#include <functional>
#include <unsupported/Eigen/AlignedVector3>

#include <Core/Math/Math.hpp>

// General config
// Use this to force vec3 to be aligned for vectorization (FIXME not working yet)
// #define CORE_USE_ALIGNED_VEC3

namespace Ra {
namespace Core {
namespace Math {
//
// Common vector types
//
using VectorN = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
using VectorNf = Eigen::VectorXf;
using VectorNd = Eigen::VectorXd;

using Vector4 = Eigen::Matrix<Scalar, 4, 1>;
using Vector4f = Eigen::Vector4f;
using Vector4d = Eigen::Vector4d;

#ifndef CORE_USE_ALIGNED_VEC3
using Vector3 = Eigen::Matrix<Scalar, 3, 1>;
using Vector3f = Eigen::Vector3f;
using Vector3d = Eigen::Vector3d;
#else
using Vector3 = Eigen::AlignedVector3<Scalar>;
using Vector3f = Eigen::AlignedVector3<float>;
using Vector3d = Eigen::AlignedVector3<double>;
#endif

using Vector2 = Eigen::Matrix<Scalar, 2, 1>;
using Vector2f = Eigen::Vector2f;
using Vector2d = Eigen::Vector2d;

using VectorNi = Eigen::VectorXi;
using Vector2i = Eigen::Vector2i;
using Vector3i = Eigen::Vector3i;
using Vector4i = Eigen::Vector4i;

using VectorNui = Eigen::Matrix<uint, Eigen::Dynamic, 1>;
using Vector2ui = Eigen::Matrix<uint, 2, 1>;
using Vector3ui = Eigen::Matrix<uint, 3, 1>;
using Vector4ui = Eigen::Matrix<uint, 4, 1>;

//
// Common matrix types
//

using MatrixN = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
using Matrix4 = Eigen::Matrix<Scalar, 4, 4>;
using Matrix3 = Eigen::Matrix<Scalar, 3, 3>;
using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;

using MatrixNf = Eigen::MatrixXf;
using Matrix4f = Eigen::Matrix4f;
using Matrix3f = Eigen::Matrix3f;
using Matrix2f = Eigen::Matrix2f;

using MatrixNd = Eigen::MatrixXd;
using Matrix4d = Eigen::Matrix4d;
using Matrix3d = Eigen::Matrix3d;
using Matrix2d = Eigen::Matrix2d;

using MatrixNui = Eigen::Matrix<uint, Eigen::Dynamic, Eigen::Dynamic>;

// using Diagonal = Eigen::DiagonalMatrix< Scalar, Eigen::Dynamic >;
using Diagonal =
    Eigen::SparseMatrix<Scalar>; // Not optimized for Diagonal matrices, but the operations between
                                 // Sparse and Diagonal are not defined
using Sparse = Eigen::SparseMatrix<Scalar>;

//
// Transforms and rotations
//

using Quaternion = Eigen::Quaternion<Scalar>;
using Quaternionf = Eigen::Quaternionf;
using Quaterniond = Eigen::Quaterniond;

using Transform = Eigen::Transform<Scalar, 3, Eigen::Affine>;
using Transformf = Eigen::Affine3f;
using Transformd = Eigen::Affine3d;

using Aabb = Eigen::AlignedBox<Scalar, 3>;
using Aabbf = Eigen::AlignedBox3f;
using Aabbd = Eigen::AlignedBox3d;

using AngleAxis = Eigen::AngleAxis<Scalar>;
using AngleAxisf = Eigen::AngleAxisf;
using AngleAxisd = Eigen::AngleAxisd;

using Translation = Eigen::Translation<Scalar, 3>;
using Translationf = Eigen::Translation3f;
using Translationd = Eigen::Translation3d;

inline void print( const MatrixN& matrix );

//
// Geometry types
//

using Line2 = Eigen::ParametrizedLine<Scalar, 2>;
using Line3 = Eigen::ParametrizedLine<Scalar, 3>;
using Plane3 = Eigen::Hyperplane<Scalar, 3>;

// Todo : storage transform using quaternions ?

//
// Misc types
//
using Color = Vector4;
using Colorf = Vector4f;
using Colord = Vector4d;

//
// Vector Functions
//
namespace Vector {

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
template <typename Vector>
inline Vector clamp( const Vector& v, const Vector& min, const Vector& max );

/// Component-wise clamp() function on a floating-point vector.
template <typename Vector>
inline Vector clamp( const Vector& v, const Scalar& min, const Scalar& max );

/// Vector range check, works for any numeric vector.
template <typename Vector_>
inline bool checkRange( const Vector_& v, const Scalar& min, const Scalar& max );

/// Call std::isnormal on vector entries.
template <typename Vector_>
inline bool checkInvalidNumbers( Eigen::Ref<const Vector_> v, const bool FAIL_ON_ASSERT = false );

/// Get two vectors orthogonal to a given vector.
inline void getOrthogonalVectors( const Vector3& fx, Eigen::Ref<Vector3> fy,
                                  Eigen::Ref<Vector3> fz );

/// Get the angle between two vectors. Works for types where the cross product is
/// defined (i.e. 2D and 3D vectors).
template <typename Vector_>
inline Scalar angle( const Vector_& v1, const Vector_& v2 );

/// Get the spherical linear interpolation between two unit non-colinear vectors.
/// works for types where the cross-product is defined (i.e. 2D and 3D vectors).
template <typename Vector_>
inline Vector_ slerp( const Vector_& v1, const Vector_& v2, Scalar t );

/// @return the projection of point on the plane define by plane and planeNormal
inline Vector3 projectOnPlane( const Vector3& planePos, const Vector3& planeNormal,
                               const Vector3& point );

/// Get the cotangent of the angle between two vectors. Works for vector types where
/// dot and cross product is defined (2D or 3D vectors).
template <typename Vector_>
inline Scalar cotan( const Vector_& v1, const Vector_& v2 );

/// Get the cosine of the angle between two vectors.
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

} // namespace Vector

namespace MatrixUtils {
inline Matrix4 lookAt( const Vector3& position, const Vector3& target, const Vector3& up );
inline Matrix4 perspective( Scalar fovy, Scalar aspect, Scalar near, Scalar zfar );
inline Matrix4 orthographic( Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar near,
                             Scalar zfar );

/// Call std::isnormal on matrix entry.
/// Dense version
template <typename Matrix_>
inline bool checkInvalidNumbers( Eigen::Ref<const Matrix_> matrix,
                                 const bool FAIL_ON_ASSERT = false );

} // namespace MatrixUtils

//
// Quaternion functions
//
namespace QuaternionUtils {
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

/// Call std::isnormal on quaternion entries.
template <typename Quaternion_>
inline bool checkInvalidNumbers( Eigen::Ref<const Quaternion_> q,
                                 const bool FAIL_ON_ASSERT = false ) {
    return MatrixUtils::checkInvalidNumbers( q.coeffs(), FAIL_ON_ASSERT );
}
} // namespace QuaternionUtils

// Use this macro in the public: section of a class
// when declaring objects containing Vector or Matrices.
// http://eigen.tuxfamily.org/dox-devel/group__TopicStructHavingEigenMembers.html
#define RA_CORE_ALIGNED_NEW EIGEN_MAKE_ALIGNED_OPERATOR_NEW
/// Use this parameter for aligning structures with Eigen members in it
#define RA_DEFAULT_ALIGN EIGEN_MAX_ALIGN_BYTES

} // namespace Math
} // namespace Core
} // namespace Ra

#include <Core/Math/LinearAlgebra.inl>

#endif // RADIUMENGINE_VECTOR_HPP
