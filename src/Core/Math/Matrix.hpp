#ifndef RADIUMENGINE_MATRIX_HPP
#define RADIUMENGINE_MATRIX_HPP

/// This file contains typedefs and basic matrix classes.
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <Core/CoreMacros.hpp>

namespace Ra { namespace Core
{

	typedef Eigen::Matrix<Scalar, 4, 4> Matrix4;
	typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
	typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;

}} // namespace Ra::Core

#endif // RADIUMENGINE_MATRIX_HPP
