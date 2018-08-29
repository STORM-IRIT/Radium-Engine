#ifndef LAPLACIAN_DEFINITION
#define LAPLACIAN_DEFINITION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>

#include <Core/Geometry/Adjacency/Adjacency.hpp>

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

// Defining the LaplacianMatrix as the sparse matrix such that: L = f( D, A )
// where f( D, A ) is a function taking in input a DegreeMatrix and an AdjacencyMatrix.
using LaplacianMatrix = Sparse;

/**
 * Return the LaplacianMatrix from the given matrices D and A.
 *
 * If POSITIVE_SEMI_DEFINITE is true then: \f$ L = D - A \f$
 * where L could/should be a positive semi-definite matrix.
 *
 * If POSITIVE_SEMI_DEFINITE is false then: \f$ L = A - D \f$
 * where L could/should be a negative semi-definite matrix.
 *
 * \warning For computing the positive semi-definite cotangent weight laplacian
 *          faster use cotangentWeightLaplacian().
 */
LaplacianMatrix RA_CORE_API standardLaplacian( const DegreeMatrix& D, const AdjacencyMatrix& A,
                                               const bool POSITIVE_SEMI_DEFINITE = true );

/**
 * Return the LaplacianMatrix from the given matrices D and A.
 *
 * The LaplacianMatrix is defined as: \f$ L = I - D^{-1/2} A D^{-1/2} \f$
 * where I is the identity matrix.
 */
LaplacianMatrix RA_CORE_API symmetricNormalizedLaplacian( const DegreeMatrix& D,
                                                          const AdjacencyMatrix& A );

/**
 * Return the LaplacianMatrix from the given matrices D and A.
 *
 * The LaplacianMatrix is defined as: \f$ L = I - D^{-1} A \f$
 * where I is the identity matrix.
 */
LaplacianMatrix RA_CORE_API randomWalkNormalizedLaplacian( const DegreeMatrix& D,
                                                           const AdjacencyMatrix& A );

/**
 * Return the LaplacianMatrix computed as the k-power of the matrix L.
 *
 * The definition was taken from:
 * "Remeshing for Multiresolution Modeling"
 * [ Mario Botsch, Leif Kobbelt ]
 * Eurographics 2004
 *
 * \warning The implementation could be wrong.
 */
LaplacianMatrix RA_CORE_API powerLaplacian( const LaplacianMatrix& L, const uint k );

/**
 * Return the LaplacianMatrix vector for the given set of points and triangles.
 *
 * The LaplacianMatrix is defined as:
 *   \f{eqnarray}{
 *       L( i, j ) &=& -&0.5 * \sum cot( \alpha_{ij} ) + cot( \beta_{ij} ) &,&
 *  \text{with } i != j \text{ and exist edge } \{i, j\} \\
 *       L( i, i ) &=&  &0.5 * \sum L( i, j ) &,& \text{with } i != j
 *   \f}
 * where \f$ \alpha_{ij} \f$ and \f$ \beta_{ij} \f$ are the angles opposite the
 * edge {i, j}, and L could/should be a positive semi-definite matrix.
 *
 * \warning This implementation is faster than doing L = D - A.
 */
LaplacianMatrix RA_CORE_API cotangentWeightLaplacian( const VectorArray<Vector3>& p,
                                                      const VectorArray<Triangle>& T );

/**
 * Return the Laplacian vector for the given point v and its one-ring.
 *
 * The Laplacian vector is defined as: \f$ L = \sum ( v - p_j ) \f$
 *
 * \warning The implementation could be wrong.
 */
Vector3 RA_CORE_API uniformLaplacian( const Vector3& v, const VectorArray<Vector3>& p );

/**
 * Return the Laplacian vector for the given point v and its one-ring.
 *
 * The Laplacian vector is defined as:
 *  \f$ L = 0.5 * \sum cot( \alpha_{vj} ) + cot( \beta_{vj} ) * ( v - p_j ) \f$
 * where \f$ \alpha_{vj} \f$ and \f$ \beta_{vj} \f$ are the angles opposite the edge {v, \f$ p_j
 * \f$}
 */
Vector3 RA_CORE_API cotangentWeightLaplacian( const Vector3& v, const VectorArray<Vector3>& p );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // LAPLACIAN_DEFINITION
