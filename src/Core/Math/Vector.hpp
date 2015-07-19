#ifndef RADIUMENGINE_VECTOR_HPP
#define RADIUMENGINE_VECTOR_HPP

/// This file contains typedefs and basic vector classes and functions

#include <cmath>
#include <functional>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <unsupported/Eigen/AlignedVector3>

#include <Core/CoreMacros.hpp>

// General config
// Use this to force vec3 to be aligned for vectorization (FIXME not working yet)
// #define CORE_USE_ALIGNED_VEC3

namespace Ra { namespace Core
{
    //
    // Common vector types
    //

    /// Our default Eigen vector type is the 4D vector.
    typedef Eigen::Matrix<Scalar, 4, 1> Vector4;
    typedef Eigen::Vector4f             Vector4f;
    typedef Eigen::Vector4d             Vector4d;

#ifndef CORE_USE_ALIGNED_VEC3
    typedef Eigen::Matrix<Scalar, 3, 1> Vector3;
    typedef Eigen::Vector3f             Vector3f;
    typedef Eigen::Vector3d             Vector3d;
#else
    typedef Eigen::AlignedVector3<Scalar> Vector3;
#endif

	typedef Eigen::Matrix<Scalar, 2, 1> Vector2; 
    typedef Eigen::Vector2f             Vector2f;
    typedef Eigen::Vector2d             Vector2d;

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


    // For consistency we redefine VectorXi in our namespace
	typedef Eigen::Vector2i Vector2i;
    typedef Eigen::Vector3i Vector3i;

	typedef Vector4 Color;

    //
    // Vector functions
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


}} // end namespace Ra::Core

#include <Core/Math/Vector.inl>

#endif// RADIUMENGINE_VECTOR_HPP

