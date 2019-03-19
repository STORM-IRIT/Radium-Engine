#ifndef HEAT_DIFFUSION
#define HEAT_DIFFUSION

#include <Core/Containers/VectorArray.hpp> // VectorArray
#include <Core/Geometry/Area.hpp>          // Geometry::AreaMatrix
#include <Core/Geometry/Laplacian.hpp>     // Geometry::LaplacianMatrix
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/** \name Heat diffusion
 * Based on:
 *
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
/// \{

/**
 * Define a vector containing the indices of the heat sources.
 */
using HeatSource = std::vector<uint8_t>;

/**
 * Define the time the heat is allowed to travel through the surface.
 */
using Time = Scalar;

/**
 * Define the vector containing the heat value of each point of the surface.
 */
using Heat = VectorArray<Scalar>;

/**
 * Return the time the heating is allowed to travel on the surface.
 * The time is defined as:
 *     \f$ t = m * h^2 \f$
 */
RA_CORE_API Time t( const Scalar& m, const Scalar& h );

/**
 * Solve the heating equation from the given AreaMatrix, the Time, the LaplacianMatrix
 * and the Delta. The heating equation is defined as:
 *     \f$ ( A + t * L )u = delta \f$
 * where \f$ u \f$ is the unknown heating after a time \f$ t \f$ has passed.
 * The equation is solved using a \f$ LL^T \f$ decomposition.
 * \warning \f$ L \f$ must be a positive semi-definite matrix.
 */
RA_CORE_API void heat( const AreaMatrix& A, const Time& t, const LaplacianMatrix& L, Heat& u,
                       const Sparse& delta );

/**
 * Solve the heating equation from the given AreaMatrix, the Time, the LaplacianMatrix
 * and the Delta. The heating equation is defined as:
 *     \f$ ( A + t * L )u = delta \f$
 * where \f$ u \f$ is the unknown heating after a time \f$ t \f$ has passed.
 * The equation is solved using a \f$ LL^T \f$ decomposition.
 * \warning \f$ L \f$ must be a positive semi-definite matrix.
 */
RA_CORE_API Heat heat( const AreaMatrix& A, const Time& t, const LaplacianMatrix& L,
                       const Sparse& delta );
/// \}

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // HEAT_DIFFUSION
