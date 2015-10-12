#ifndef LAPLACIAN_DEFINITION
#define LAPLACIAN_DEFINITION

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/**
* For further reading on how to compute the Laplacian over a triangle mesh
* a suggested reading is:
*
* "Laplacian Mesh Processing"
* [ Olga Sorline ]
* Eurographics 2005
*
*
* Different approaches on data other than triangle meshes could be found in:
*
* "Discrete Laplacians On General Polygonal Meshes"
* [ Mark Alexa, Max Wardetzky ]
* TOG 2011
*
* "Point-Based Manifold Harmonics"
* [ Yang Liu, Balakrishnan Prabhakaran, Xiaohu Guo ]
* Visualization and Computer Graphics 2012
*
**/

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



/*
* Return the AdjacencyMatrix for the given set of points and triangles.
* The function defined over the edges is:
*       f( i, j ) = 1 , if exist the edge from i to j
*       f( i, j ) = 0 , otherwise
*/
AdjacencyMatrix uniformAdjacency( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T );



/*
* Return the AdjacencyMatrix for the given set of points and triangles.
* The function defined over the edges is:
*       f( i, j ) = 0.5 * ( cot( alpha_ij ) + cot( beta_ij ) ) , if exist the edge from i to j
*       f( i, j ) = 0                                          , otherwise
* where alpha_ij and beta_ij are the angles opposite the edge
*/
AdjacencyMatrix cotangentWeightAdjacency( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T );



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
DegreeMatrix adjacencyDegree( const AdjacencyMatrix& A ) ;



// //////////////// //
// LAPLACIAN MATRIX //
// //////////////// //

// Defining the LaplacianMatrix as the sparse matrix such that:
//      L = f( D, A )
// where f( D, A ) is a function taking in input a DegreeMatrix and an AdjacencyMatrix
typedef Sparse LaplacianMatrix;



/*
* Return the LaplacianMatrix from the given matrices D and A.
*
* If POSITIVE_SEMI_DEFINITE is true then:
*        L = D - A
* where L could/should be a positive semi-definite matrix.
*
* If POSITIVE_SEMI_DEFINITE is false then:
*        L = A - D
* where L could/should be a negative semi-definite matrix.
*/
/// WARNING; FOR COMPUTING THE POSITIVE SEMI-DEFINITE COTANGENT WEIGHT LAPLACIAN FASTER USE cotangentWeightLaplacian
LaplacianMatrix standardLaplacian( const DegreeMatrix& D, const AdjacencyMatrix& A, const bool POSITIVE_SEMI_DEFINITE = true );



/*
* Return the LaplacianMatrix from the given matrices D and A.
*
* The LaplacianMatrix is defined as:
*       L = I - D^-1/2 A D^-1/2
* where I is the identity matrix.
*/
LaplacianMatrix symmetricNormalizedLaplacian( const DegreeMatrix& D, const AdjacencyMatrix& A );



/*
* Return the LaplacianMatrix from the given matrices D and A.
*
* The LaplacianMatrix is defined as:
*       L = I - D^-1 A
* where I is the identity matrix.
*/
LaplacianMatrix randomWalkNormalizedLaplacian( const DegreeMatrix& D, const AdjacencyMatrix& A );



/*
* Return the LaplacianMatrix computed as the k-power of the matrix L.
*
* The definition was taken from:
* "Remeshing for Multiresolution Modeling"
* [ Mario Botsch, Leif Kobbelt ]
* Eurographics 2004
*/
/// WARNING: THE IMPLEMENTATION COULD BE WRONG
LaplacianMatrix powerLaplacian( const LaplacianMatrix& L, const uint k );



/*
* Return the LaplacianMatrix vector for the given set of points and triangles.
*
* The LaplacianMatrix is defined as:
*       L( i, j ) = -0.5 * sum( cot( alpha_ij ) + cot( beta_ij ) )  , with i != j and exist edge { i, j }
*       L( i, i ) =  0.5 * sum( L( i, j ) )                         , with i != j
* where alpha_ij and beta_ij are the angles opposite the edge { i, j },
* and L could/should be a positive semi-definite matrix.
*/
/// WARNING: THIS IMPLEMENTATION IS FASTER THAN DOING L = D - A.
LaplacianMatrix cotangentWeightLaplacian( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T );



////////////////
/// ONE RING ///
////////////////

/*
* Return the laplacian vector for the given point v and its one-ring.
*
* The laplacian vector is defined as:
*       L = sum( ( v - p_j ) )
*/
/// WARNING: THE IMPLEMENTATION COULD BE WRONG
Vector3 uniformLaplacian( const Vector3& v, const VectorArray< Vector3 >& p );



/*
* Return the laplacian vector for the given point v and its one-ring.
*
* The laplacian vector is defined as:
*       L = 0.5 * sum( ( cot( alpha_vj ) + cot( beta_vj ) ) * ( v - p_j ) )
* where alpha_ij and beta_ij are the angles opposite the edge { v, p_j }
*/
Vector3 cotangentWeightLaplacian( const Vector3& v, const VectorArray< Vector3 >& p );



}
}
}

#endif // LAPLACIAN_DEFINITION
