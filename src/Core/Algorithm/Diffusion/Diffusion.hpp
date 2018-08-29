#ifndef DIFFUSION_DEFINITION
#define DIFFUSION_DEFINITION

#include <Core/Algorithm/Delta/Delta.hpp>        // Delta
#include <Core/Geometry/Adjacency/Adjacency.hpp> //Geometry::AdjacencyMatrix
#include <Core/Math/LinearAlgebra.hpp>           // Sparse

namespace Ra {
namespace Core {
namespace Algorithm {

// Defining the vector of scalar values over the vertices
using ScalarValue = Sparse;

/**
 * Return the scalar value over a mesh after diffusion.
 * Given the AdjacencyMatrix of the mesh, a Delta, a factor \f$ \lambda \f$ and
 * the number of iteration to be performed, the scalar \f$ u_i \f$ related to
 * the vertex \f$ v_i \f$ is computed as:
 *   \f$ u_i = ( \lambda * C_i ) + ( Delta_i * ( 1 - \lambda ) ) \f$
 *
 * where \f$ C_i \f$ is the value of the centroid of the one-ring of vertex \f$ v_i \f$.
 *
 * \warning The implementation could be wrong.
 */
RA_CORE_API ScalarValue diffuseDelta( const Geometry::AdjacencyMatrix& A, const Delta& delta,
                                      const Scalar lambda, const uint iteration );

/**
 * Return the scalar value over a mesh after diffusion.
 * Given the AdjacencyMatrix of the mesh, a Delta, a factor \f$ \lambda \f$ and
 * the number of iteration to be performed, the scalar \f$ u_i \f$ related to
 * the vertex \f$ v_i \f$ is computed as:
 *   \f$ u_i = ( \lambda * C_i ) + ( Delta_i * ( 1 - \lambda ) ) \f$
 *
 * where \f$ C_i \f$ is the value of the centroid of the one-ring of vertex \f$ v_i \f$.
 *
 * \warning The implementation could be wrong.
 */
RA_CORE_API void diffuseDelta( const Geometry::AdjacencyMatrix& A, const Delta& delta,
                               const Scalar lambda, const uint iteration, ScalarValue& value );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // DIFFUSION_DEFINITION
