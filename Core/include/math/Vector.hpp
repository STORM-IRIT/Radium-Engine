#ifndef RADIUMENGINE_VECTOR_HPP_
#define RADIUMENGINE_VECTOR_HPP_

/// This file contains typedefs and basic vector classes.
/// Internally, the underlying storage class for 3D vectors and 4D vectors are Eigen's vector4;
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <unsupported/Eigen/AlignedVector3>

#include <CoreMacros.hpp>


// General config
// Use this to use double precision for all vectors
// #define CORE_USE_DOUBLE

namespace Ra {

#ifndef CORE_USE_DOUBLE
    typedef float Scalar;

    /// Our default Eigen vector type is the 4D vector.
    typedef Eigen::Vector4f Vector4;

    /// We will use an aligned vector for 3D vectors.
    ///typedef Eigen::AlignedVector3<float> Vector3;
    typedef Eigen::Vector3f Vector3;

    typedef Eigen::Quaternionf Quaternion;

    typedef Eigen::Affine3f Transform;
    // Todo : storage transform using quaternions ?
#else //
    typedef double Scalar;

    typedef Eigen::Vector4d Vector4;

    //typedef Eigen::AlignedVector3<double> Vector3;
    typedef Eigen::Vector3d Vector3;

    typedef Eigen::Quaterniond Quaternion;

    typedef Eigen::Affine3d Transform;


#endif
} // end namespace Ra

#endif// RADIUMENGINE_VECTOR_HPP_
