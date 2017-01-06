#ifndef RADIUMENGINE_VECTOR_HPP
#define RADIUMENGINE_VECTOR_HPP

/// This file contains typedefs and basic vector classes and functions

#include <Core/RaCore.hpp>

#include <functional>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/Geometry>
#include <unsupported/Eigen/AlignedVector3>

#include <Core/Math/Math.hpp>

// General config
// Use this to force vec3 to be aligned for vectorization (FIXME not working yet)
// #define CORE_USE_ALIGNED_VEC3

namespace Ra
{
    namespace Core
    {
        //
        // Common vector types
        //
        typedef Eigen::Matrix< Scalar, Eigen::Dynamic, 1 > VectorN;
        typedef Eigen::VectorXf                            VectorNf;
        typedef Eigen::VectorXd                            VectorNd;

        typedef Eigen::Matrix<Scalar, 4, 1> Vector4;
        typedef Eigen::Vector4f             Vector4f;
        typedef Eigen::Vector4d             Vector4d;

#ifndef CORE_USE_ALIGNED_VEC3
        typedef Eigen::Matrix<Scalar, 3, 1> Vector3;
        typedef Eigen::Vector3f             Vector3f;
        typedef Eigen::Vector3d             Vector3d;
#else
        typedef Eigen::AlignedVector3<Scalar> Vector3;
        typedef Eigen::AlignedVector3<float> Vector3f;
        typedef Eigen::AlignedVector3<double> Vector3d;
#endif

        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
        typedef Eigen::Vector2f             Vector2f;
        typedef Eigen::Vector2d             Vector2d;

        typedef Eigen::VectorXi VectorNi;
        typedef Eigen::Vector2i Vector2i;
        typedef Eigen::Vector3i Vector3i;
        typedef Eigen::Vector4i Vector4i;


        typedef Eigen::Matrix<uint, Eigen::Dynamic, 1> VectorNui;
        typedef Eigen::Matrix<uint, 2, 1>              Vector2ui;
        typedef Eigen::Matrix<uint, 3, 1>              Vector3ui;
        typedef Eigen::Matrix<uint, 4, 1>              Vector4ui;

        //
        // Common matrix types
        //

        typedef Eigen::Matrix< Scalar, Eigen::Dynamic, Eigen::Dynamic > MatrixN;
        typedef Eigen::Matrix<Scalar, 4, 4> Matrix4;
        typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
        typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;

        typedef Eigen::MatrixXf MatrixNf;
        typedef Eigen::Matrix4f Matrix4f;
        typedef Eigen::Matrix3f Matrix3f;
        typedef Eigen::Matrix2f Matrix2f;

        typedef Eigen::MatrixXd MatrixNd;
        typedef Eigen::Matrix4d Matrix4d;
        typedef Eigen::Matrix3d Matrix3d;
        typedef Eigen::Matrix2d Matrix2d;

        typedef Eigen::Matrix< uint, Eigen::Dynamic, Eigen::Dynamic > MatrixNui;

        //typedef Eigen::DiagonalMatrix< Scalar, Eigen::Dynamic > Diagonal;
        typedef Eigen::SparseMatrix< Scalar > Diagonal; // Not optimized for Diagonal matrices, but the operations between Sparse and Diagonal are not defined
        typedef Eigen::SparseMatrix< Scalar > Sparse;

        //
        // Transforms and rotations
        //

        typedef Eigen::Quaternion<Scalar> Quaternion;
        typedef Eigen::Quaternionf        Quaternionf;
        typedef Eigen::Quaterniond        Quaterniond;

        typedef Eigen::Transform<Scalar, 3, Eigen::Affine> Transform;
        typedef Eigen::Affine3f                            Transformf;
        typedef Eigen::Affine3d                            Transformd;

        typedef Eigen::AlignedBox<Scalar, 3> Aabb;
        typedef Eigen::AlignedBox3f          Aabbf;
        typedef Eigen::AlignedBox3d          Aabbd;

        typedef Eigen::AngleAxis<Scalar> AngleAxis;
        typedef Eigen::AngleAxisf        AngleAxisf;
        typedef Eigen::AngleAxisd        AngleAxisd;

        typedef Eigen::Translation<Scalar, 3> Translation;
        typedef Eigen::Translation3f         Translationf;
        typedef Eigen::Translation3d         Translationd;

        inline void print( const MatrixN& matrix );

        //
        // Geometry types
        //

        typedef Eigen::ParametrizedLine< Scalar, 2 > Line2;
        typedef Eigen::ParametrizedLine< Scalar, 3 > Line3;
        typedef Eigen::Hyperplane< Scalar, 3 >       Plane3;

        // Todo : storage transform using quaternions ?

        //
        // Misc types
        //
        typedef Vector4  Color;
        typedef Vector4f Colorf;
        typedef Vector4d Colord;

        //
        // Vector Functions
        //
        namespace Vector
        {
            /// Component-wise floor() function on a floating-point vector.
            template<typename Vector>
            inline Vector floor( const Vector& v );

            /// Component-wise ceil() function on a floating-point vector.
            template<typename Vector>
            inline Vector ceil( const Vector& v );

            /// Component-wise clamp() function on a floating-point vector.
            template<typename Vector>
            inline Vector clamp( const Vector& v, const Vector& min, const Vector& max );

            /// Component-wise clamp() function on a floating-point vector.
            template<typename Vector>
            inline Vector clamp( const Vector& v, const Scalar& min, const Scalar& max );

            /// Vector range check, works for any numeric vector.
            template<typename Vector_>
            inline bool checkRange( const Vector_& v, const Scalar& min, const Scalar& max );

            /// Get two vectors orthogonal to a given vector.
            inline void getOrthogonalVectors( const Vector3& fx, Vector3& fy, Vector3& fz );

            /// Get the angle between two vectors. Works for types where the cross product is
            /// defined (i.e. 2D and 3D vectors).
            template<typename Vector_>
            inline Scalar angle( const Vector_& v1, const Vector_& v2);

            /// @return the projection of point on the plane define by planePos and planeNormal
            inline Vector3 projectOnPlane(const Vector3& planePos, const Vector3 planeNormal, const Vector3& point);

            /// Get the cotangent of the angle between two vectors. Works for vector types where
            /// dot and cross product is defined (2D or 3D vectors).
            template <typename Vector_>
            inline Scalar cotan( const Vector_& v1, const Vector_& v2);

            /// Get the cosine of the angle between two vectors.
            template <typename Vector_>
            inline Scalar cos( const Vector_& v1, const Vector_& v2);
        }

        namespace MatrixUtils
        {
            inline Matrix4 lookAt(const Vector3& position, const Vector3& target, const Vector3& up);
            inline Matrix4 perspective(Scalar fovy, Scalar aspect, Scalar near, Scalar zfar);
            inline Matrix4 orthographic(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar near, Scalar zfar);
        }

        //
        // Quaternion functions
        //
        namespace QuaternionUtils
        {
            // Define functions for multiplying a quaternion by a scalar
            // and adding two quaternions. While Quaternion is supposed to
            // represent a unit quaternion (thus a valid rotation), these functions
            // are useful for linear interpolation of quaternions.

            /// Returns the quaternion q multipled by a scalar factor of k.
            inline Quaternion scale(const Quaternion& q, const Scalar k);

            /// Returns the sum of two quaternions.
            inline Quaternion add(const Quaternion& q1, const Quaternion& q2);

            /// Returns the sum of two quaternions, resolving antipodality by flipping
            /// the sign of q2 if q1.q2 is negative. This operation is usually
            /// denoted as a circled + sign, forming the basis of the QLERP algorithm.
            /// See "Spherical Blend Skinning" (Kavan & Zara 2005) for more details.
            inline Quaternion addQlerp(const Quaternion& q1, const Quaternion& q2);

            // Note : the .inl file also define operator+ for quaternions
            // and operator * and / between quaternions and scalar.

            /// Decompose a given rotation Qin into a swing rotation and a twist rotation.
            /// Qswing is a rotation whose axis lies in the XY plane and Qtwist is a rotation about axis Z.
            /// such as Qin = Qswing * Qtwist
            /// If the rotation is already around axis z, Qswing will be set to identity
            /// and Qtwist equal to Qin
            inline void getSwingTwist(const Quaternion& in, Quaternion& swingOut, Quaternion& twistOut);
        }


        // Use this macro in the public: section of a class
        // when declaring objects containing Vector or Matrices.
        // http://eigen.tuxfamily.org/dox-devel/group__TopicStructHavingEigenMembers.html
#define  RA_CORE_ALIGNED_NEW EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    }
} // end namespace Ra::Core

#include <Core/Math/LinearAlgebra.inl>

#endif// RADIUMENGINE_VECTOR_HPP

