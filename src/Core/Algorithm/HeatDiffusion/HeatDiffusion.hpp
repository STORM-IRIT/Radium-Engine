#ifndef HEAT_DIFFUSION
#define HEAT_DIFFUSION

#include <Core/Containers/VectorArray.hpp>       // VectorArray
#include <Core/Geometry/Area/Area.hpp>           // Geometry::AreaMatrix
#include <Core/Geometry/Laplacian/Laplacian.hpp> // Geometry::LaplacianMatrix
#include <Core/Algorithm/Delta/Delta.hpp>        // Delta
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

// Defining a vector containing the indices of the heat sources
using HeatSource = Source;

// Defining the time the heat is allowed to travel through the surface
using Time = Scalar;

// Defining the vector containing the heat value of each point of the surface
using Heat = VectorArray<Scalar>;
// using  Heat = Eigen::Matrix< Scalar, Eigen::Dynamic, 1 >;

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
RA_CORE_API Time t( const Scalar& m, const Scalar& h );

/*
 * Solve the heating equation from the given AreaMatrix, the Time, the LaplacianMatrix and the
 * Delta. The heating equation is defined as: ( A + t * L )u = delta where u is the unknown heating
 * after a time t has passed. The equation is solved using a LL^T decomposition.
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
/// WARNING: L must be a positive semi-definite matrix
RA_CORE_API void heat( const Geometry::AreaMatrix& A, const Time& t,
                       const Geometry::LaplacianMatrix& L, Heat& u, const Delta& delta );

/*
 * Solve the heating equation from the given AreaMatrix, the Time, the LaplacianMatrix and the
 * Delta. The heating equation is defined as: ( A + t * L )u = delta where u is the unknown heating
 * after a time t has passed. The equation is solved using a LL^T decomposition.
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
/// WARNING: L must be a positive semi-definite matrix
RA_CORE_API Heat heat( const Geometry::AreaMatrix& A, const Time& t,
                       const Geometry::LaplacianMatrix& L, const Delta& delta );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // HEAT_DIFFUSION
