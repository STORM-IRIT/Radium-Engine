#ifndef ADJACENCY_DEFINITION
#define ADJACENCY_DEFINITION

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshTypes.hpp>

#include <Core/Utils/Graph/AdjacencyList.hpp>

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
typedef Sparse AdjacencyMatrix;
typedef AdjacencyMatrix VVAdj;
typedef AdjacencyMatrix VTAdj;
typedef AdjacencyMatrix TVAdj;



/*
* Return the AdjacencyMatrix for the given set of points and triangles.
* The function defined over the edges is:
*       f( i, j ) = 1 , if exist the edge from i to j
*       f( i, j ) = 0 , otherwise
*/
RA_CORE_API AdjacencyMatrix uniformAdjacency( const uint point_size, const VectorArray< Triangle >& T );



/*
* Return the AdjacencyMatrix for the given set of points and triangles.
* The function defined over the edges is:
*       f( i, j ) = 1 , if exist the edge from i to j
*       f( i, j ) = 0 , otherwise
*/
RA_CORE_API AdjacencyMatrix uniformAdjacency( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T );



/*
* Return the AdjacencyMatrix for the given set of points and triangles.
* The function defined over the triangle is:
*       f( i, j ) = 1 , if triangle i contains to vertex j
*       f( i, j ) = 0 , otherwise
*/
RA_CORE_API TVAdj triangleUniformAdjacency( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T );


/*
* Return the AdjacencyMatrix Adj for the given set of points and triangles.
* The function defined over the edges is:
*       f( i, j ) = 1 , if exist the edge from i to j
*       f( i, j ) = 0 , otherwise
*/
void uniformAdjacency( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, AdjacencyMatrix& Adj );


/*
* Return the AdjacencyMatrix for the given set of points and triangles.
* The function defined over the edges is:
*       f( i, j ) = 0.5 * ( cot( alpha_ij ) + cot( beta_ij ) ) , if exist the edge from i to j
*       f( i, j ) = 0                                          , otherwise
* where alpha_ij and beta_ij are the angles opposite the edge
*/
RA_CORE_API AdjacencyMatrix cotangentWeightAdjacency( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T );



// ///////////// //
// DEGREE MATRIX //
// ///////////// //

// Defining the DegreeMatrix as the diagonal matrix such that:
//      D( i, j ) = sum( A( i ) ) , if i == j
//      D( i, j ) = 0             , if i != j
// where A( i ) is the i-th row of a AdjacencyMatrix
typedef Diagonal DegreeMatrix;



/*
* Return the DegreeMatrix of the given AdjacencyMatrix A.
*/
RA_CORE_API DegreeMatrix adjacencyDegree( const AdjacencyMatrix& A ) ;



////////////////
/// ONE RING ///
////////////////

//typedef Graph::ChildrenList OneRing;

//OneRing vertexOneRing( const uint v, const VectorArray< Triangle >& T );

//Graph::Adjacency meshOneRing( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T );


}
}
}

#endif // ADJACENCY_DEFINITION
