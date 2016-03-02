#ifndef HEAT_DIFFUSION
#define HEAT_DIFFUSION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Core/Geometry/Area/Area.hpp>
#include <Core/Geometry/Laplacian/Laplacian.hpp>

#include <Core/Algorithm/Delta/Delta.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

// Defining a vector containing the indices of the heat sources
typedef Source HeatSource;

// Defining the time the heat is allowed to travel through the surface
typedef Scalar Time;

// Defining the vector containing the heat value of each point of the surface
typedef VectorArray< Scalar > Heat;
//typedef Eigen::Matrix< Scalar, Eigen::Dynamic, 1 > Heat;



/*
* Return the time the heating is allowed to travel on the surface.
* The time is defined as:
*       t = m * h^2
*
* The definition was taken from:
* "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
* [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
* TOG 2013
*/
Time t( const Scalar& m, const Scalar& h );



/*
* Solve the heating equation from the given AreaMatrix, the Time, the LaplacianMatrix and the Delta.
* The heating equation is defined as:
*       ( A + t * L )u = delta
* where u is the unknown heating after a time t has passed.
* The equation is solved using a LL^T decomposition.
*
* The definition was taken from:
* "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
* [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
* TOG 2013
*/
/// WARNING: L must be a positive semi-definite matrix
void heat( const Geometry::AreaMatrix& A, const Time& t, const Geometry::LaplacianMatrix& L, Heat& u, const Delta& delta );



/*
* Solve the heating equation from the given AreaMatrix, the Time, the LaplacianMatrix and the Delta.
* The heating equation is defined as:
*       ( A + t * L )u = delta
* where u is the unknown heating after a time t has passed.
* The equation is solved using a LL^T decomposition.
*
* The definition was taken from:
* "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
* [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
* TOG 2013
*/
/// WARNING: L must be a positive semi-definite matrix
Heat heat( const Geometry::AreaMatrix& A, const Time& t, const Geometry::LaplacianMatrix& L, const Delta& delta );



}
}
}

#endif //HEAT_DIFFUSION
