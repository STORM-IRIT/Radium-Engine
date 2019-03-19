#ifndef ADJACENCY_DEFINITION
#define ADJACENCY_DEFINITION

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// \name Adjacency Matrices
/// \{

/**
 * Define the AdjacencyMatrix as the sparse matrix such that:
 *   A( i, j ) = f( i, j )
 * where f( i, j ) is a generic function defined over a connection.
 */
using AdjacencyMatrix = Ra::Core::Sparse;

/**
 * Type for Vertex-Vertex Adjacency Matrices.
 */
using VVAdj = AdjacencyMatrix;

/**
 * Type for Vertex-Triangle Adjacency Matrices.
 */
using VTAdj = AdjacencyMatrix;

/**
 * Type for Triangle-Vertex Adjacency Matrices.
 */
using TVAdj = AdjacencyMatrix;

/**
 * Return the AdjacencyMatrix for the given set of points and triangles.
 * The function is defined over the edges as:
 *   - f( i, j ) = 1 , if there exist an edge from i to j
 *   - f( i, j ) = 0 , otherwise
 */
RA_CORE_API AdjacencyMatrix uniformAdjacency( const uint point_size,
                                              const VectorArray<Vector3ui>& T );

/**
 * Return the AdjacencyMatrix for the given set of points and triangles.
 * The function is defined over the edges as:
 *   - f( i, j ) = 1 , if there exist an edge from i to j
 *   - f( i, j ) = 0 , otherwise
 */
RA_CORE_API AdjacencyMatrix uniformAdjacency( const VectorArray<Vector3>& p,
                                              const VectorArray<Vector3ui>& T );

/**
 * Return the AdjacencyMatrix Adj for the given set of points and triangles.
 * The function defined over the edges is:
 *   - f( i, j ) = 1 , if there exist an edge from i to j
 *   - f( i, j ) = 0 , otherwise
 */
RA_CORE_API void uniformAdjacency( const VectorArray<Vector3>& p, const VectorArray<Vector3ui>& T,
                                   AdjacencyMatrix& Adj );

/**
 * Return the AdjacencyMatrix for the given set of points and triangles.
 * The function is defined over the Triangles as:
 *   - f( i, j ) = 1 , if Triangle i contains vertex j
 *   - f( i, j ) = 0 , otherwise
 */
RA_CORE_API TVAdj triangleUniformAdjacency( const VectorArray<Vector3>& p,
                                            const VectorArray<Vector3ui>& T );

/**
 * Return the Cotangent Weight Matrix for the given set of points and triangles.
 * The function defined over the edges is:
 *   - f( i, j ) = \f$ 0.5 * ( cot( \alpha_{ij} ) + cot( \beta{ij} ) ) \f$,
 *                 if there exist an edge from i to j
 *   - f( i, j ) = 0 , otherwise
 *
 * where \f$ \alpha_{ij} \f$ and \f$ \beta_{ij} \f$ are the angles opposite the edge.
 */
RA_CORE_API AdjacencyMatrix cotangentWeightAdjacency( const VectorArray<Vector3>& p,
                                                      const VectorArray<Vector3ui>& T );

/**
 * Define the DegreeMatrix as the diagonal matrix such that:
 *    - D( i, j ) = \f$ \sum \f$ A( i ) , if i == j
 *    - D( i, j ) = 0             , if i != j
 *
 *  where A( i ) is the i-th row of an AdjacencyMatrix.
 */
using DegreeMatrix = Diagonal;

/**
 * Return the DegreeMatrix of the given AdjacencyMatrix A.
 */
RA_CORE_API DegreeMatrix adjacencyDegree( const AdjacencyMatrix& A );
/// \}

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // ADJACENCY_DEFINITION
