#ifndef RADIUMENGINE_VECTOR_HPP_
#define RADIUMENGINE_VECTOR_HPP_

/// This file contains typedefs and basic vector classes and functions

#include <cmath>
#include <functional>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <unsupported/Eigen/AlignedVector3>

#include <CoreMacros.hpp>


// General config
// Use this to use double precision for all vectors
// #define CORE_USE_DOUBLE
// Use this to force vec3 to be aligned for vectorization (FIXME not working yet)
// #define CORE_USE_ALIGNED_VEC3

namespace Ra
{
    //
    // Common vector types
    //

#ifndef CORE_USE_DOUBLE
    typedef float Scalar;

    /// Our default Eigen vector type is the 4D vector.
    typedef Eigen::Vector4f Vector4;

#ifndef CORE_USE_ALIGNED_VEC3
    typedef Eigen::Vector3f Vector3;
#else
    typedef Eigen::AlignedVector3<float> Vector3;
#endif
    typedef Eigen::Quaternionf Quaternion;

    typedef Eigen::Affine3f Transform;

    typedef Eigen::AlignedBox3f Aabb;
    // Todo : storage transform using quaternions ?
#else //
    typedef double Scalar;

    typedef Eigen::Vector4d Vector4;

#ifndef CORE_USE_ALIGNED_VEC3
    typedef Eigen::Vector3f Vector3;
#else
    typedef Eigen::AlignedVector3<float> Vector3;
#endif

    typedef Eigen::Quaterniond Quaternion;

    typedef Eigen::Affine3d Transform;

    typedef Eigen::AlignedBox3d Aabb;

#endif

    // For consistency we redefine vector3i in our namespace

    typedef Eigen::Vector3i Vector3i;


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

} // end namespace Ra

#include <math/Vector.inl>

#endif// RADIUMENGINE_VECTOR_HPP_
