#ifndef ADJACENCY_DEFINITION
#define ADJACENCY_DEFINITION

#include <Core/Container/AlignedStdVector.hpp>
#include <Core/Container/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Geometry/MeshTypes.hpp>

#include <Core/Utils/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/////////////////////
/// GLOBAL MATRIX ///
/////////////////////

// //////////////// //
// ADJACENCY MATRIX //
// //////////////// //

// Defining the AdjacencyMatrix as the sparse matrix such that:
//      A( i, j ) = f( i, j ) , if exist the edge from i to j
//      A( i, j ) = 0         , otherwise
// where f( i, j ) is a generic function defined over an edge
using AdjacencyMatrix = Ra::Core::Math::Sparse;
using VVAdj = AdjacencyMatrix;
using VTAdj = AdjacencyMatrix;
using TVAdj = AdjacencyMatrix;

/*
 * Return the AdjacencyMatrix for the given set of points and triangles.
 * The function defined over the edges is:
 *       f( i, j ) = 1 , if exist the edge from i to j
 *       f( i, j ) = 0 , otherwise
 */
RA_CORE_API AdjacencyMatrix uniformAdjacency( const uint point_size,
                                              const Container::VectorArray<Triangle>& T );

/*
 * Return the AdjacencyMatrix for the given set of points and triangles.
 * The function defined over the edges is:
 *       f( i, j ) = 1 , if exist the edge from i to j
 *       f( i, j ) = 0 , otherwise
 */
RA_CORE_API AdjacencyMatrix uniformAdjacency( const Container::VectorArray<Math::Vector3>& p,
                                              const Container::VectorArray<Triangle>& T );

/*
 * Return the AdjacencyMatrix for the given set of points and triangles.
 * The function defined over the triangle is:
 *       f( i, j ) = 1 , if triangle i contains to vertex j
 *       f( i, j ) = 0 , otherwise
 */
RA_CORE_API TVAdj triangleUniformAdjacency( const Container::VectorArray<Math::Vector3>& p,
                                            const Container::VectorArray<Triangle>& T );

/*
 * Return the AdjacencyMatrix Adj for the given set of points and triangles.
 * The function defined over the edges is:
 *       f( i, j ) = 1 , if exist the edge from i to j
 *       f( i, j ) = 0 , otherwise
 */
void uniformAdjacency( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T,
                       AdjacencyMatrix& Adj );

/*
 * Return the AdjacencyMatrix for the given set of points and triangles.
 * The function defined over the edges is:
 *       f( i, j ) = 0.5 * ( cot( alpha_ij ) + cot( beta_ij ) ) , if exist the edge from i to j
 *       f( i, j ) = 0                                          , otherwise
 * where alpha_ij and beta_ij are the angles opposite the edge
 */
RA_CORE_API AdjacencyMatrix cotangentWeightAdjacency( const Container::VectorArray<Math::Vector3>& p,
                                                      const Container::VectorArray<Triangle>& T );

// ///////////// //
// DEGREE MATRIX //
// ///////////// //

// Defining the DegreeMatrix as the diagonal matrix such that:
//      D( i, j ) = sum( A( i ) ) , if i == j
//      D( i, j ) = 0             , if i != j
// where A( i ) is the i-th row of a AdjacencyMatrix
using DegreeMatrix = Math::Diagonal;

/*
 * Return the DegreeMatrix of the given AdjacencyMatrix A.
 */
RA_CORE_API DegreeMatrix adjacencyDegree( const AdjacencyMatrix& A );

////////////////
/// ONE RING ///
////////////////

// using OneRing = Utils::ChildrenList;

// OneRing vertexOneRing( const uint v, const Container::VectorArray< Triangle >& T );

// Utils::Adjacency meshOneRing( const Container::VectorArray< Math::Vector3 >& p, const Container::VectorArray< Triangle >& T
// );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // ADJACENCY_DEFINITION
