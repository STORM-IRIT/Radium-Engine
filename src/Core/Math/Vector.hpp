#ifndef RADIUMENGINE_VECTOR_HPP
#define RADIUMENGINE_VECTOR_HPP

/// This file contains typedefs and basic vector classes and functions

#include <cmath>
#include <functional>

#include <Core/CoreMacros.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <unsupported/Eigen/AlignedVector3>



// General config
// Use this to force vec3 to be aligned for vectorization (FIXME not working yet)
// #define CORE_USE_ALIGNED_VEC3

namespace Ra { namespace Core
{
    //
    // Common vector types
    //

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

    typedef Eigen::Vector2i Vector2i;
    typedef Eigen::Vector3i Vector3i;
    typedef Eigen::Vector4i Vector4i;

    //
    // Common matrix types
    //

    typedef Eigen::Matrix<Scalar, 4, 4> Matrix4;
	typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
	typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;

    typedef Eigen::Matrix4f Matrix4f;
    typedef Eigen::Matrix3f Matrix3f;
    typedef Eigen::Matrix2f Matrix2f;

    typedef Eigen::Matrix4d Matrix4d;
    typedef Eigen::Matrix3d Matrix3d;
    typedef Eigen::Matrix2d Matrix2d;

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

    typedef Eigen::Translation<Scalar,3> Translation;
    typedef Eigen::Translation3f 		 Translationf;
    typedef Eigen::Translation3d 		 Translationd;

// Todo : storage transform using quaternions ?

    //
    // Misc types
	//
    typedef Vector4 Color;

    //
    // Vector Functions
    //
    namespace Vector
    {
        /// Component-wise floor() function on a floating-point vector.
        template<typename Vector>
        inline Vector floor(const Vector& v);

        /// Component-wise floor() function on a floating-point vector.
        template<typename Vector>
        inline Vector ceil(const Vector& v);

        /// Component-wise clamp() function on a floating-point vector.
        template<typename Vector>
        inline Vector clamp(const Vector& v, const Vector& min, const Vector& max);
    }

    //
    // Quaternion functions
    //

    inline Quaternion operator+ (const Quaternion& q1, const Quaternion& q2);
    inline Quaternion operator* (const Scalar& k, const Quaternion& q);


    // Use this macro in the public: section of a class
    // when declaring objects containing Vector or Matrices.
    // http://eigen.tuxfamily.org/dox-devel/group__TopicStructHavingEigenMembers.html
#define  RA_CORE_ALIGNED_NEW EIGEN_MAKE_ALIGNED_OPERATOR_NEW

}} // end namespace Ra::Core

#include <Core/Math/Vector.inl>

#endif// RADIUMENGINE_VECTOR_HPP

