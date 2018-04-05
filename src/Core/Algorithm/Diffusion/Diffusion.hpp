#ifndef DIFFUSION_DEFINITION
#define DIFFUSION_DEFINITION

#include <Core/Math/LinearAlgebra.hpp>    // Sparse
#include <Core/Algorithm/Delta/Delta.hpp> // Delta
#include <Core/Geometry/Adjacency/Adjacency.hpp> //Geometry::AdjacencyMatrix

namespace Ra {
namespace Core {
namespace Algorithm {

// Defining the vector of scalar values over the vertices
using ScalarValue = Sparse;

/*
 * Return the scalar value over a mesh after diffusion.
 * Given the AdjacencyMatrix of the mesh, a Delta, a factor lambda and the number of iteration to be
 * performed, the scalar u_i related to vertex v_i is computed as:
 *
 *       u_i = ( lambda * C_i ) + ( delta_i * ( 1 - lambda ) )
 *
 * where C_i is the value of the centroid of v_i one-ring.
 */
/// WARNING: THE IMPLEMENTATION COULD BE WRONG
RA_CORE_API ScalarValue diffuseDelta( const Geometry::AdjacencyMatrix& A,
                                      const Delta& delta,
                                      const Scalar lambda,
                                      const uint iteration );

/*
 * Return the scalar value over a mesh after diffusion.
 * Given the AdjacencyMatrix of the mesh, a Delta, a factor lambda and the number of iteration to be
 * performed, the scalar u_i related to vertex v_i is computed as:
 *
 *       u_i = ( lambda * C_i ) + ( delta_i * ( 1 - lambda ) )
 *
 * where C_i is the value of the centroid of v_i one-ring.
 */
/// WARNING: THE IMPLEMENTATION COULD BE WRONG
RA_CORE_API void diffuseDelta( const Geometry::AdjacencyMatrix& A,
                               const Delta& delta,
                               const Scalar lambda,
                               const uint iteration,
                               ScalarValue& value );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // DIFFUSION_DEFINITION
