#ifndef CURVATURE_DEFINITION
#define CURVATURE_DEFINITION

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Area/Area.hpp>

namespace Ra {
namespace Core {
namespace Geometry {



// Typedef
typedef Scalar MeanCurvature;
typedef Scalar GaussianCurvature;
typedef Scalar MaximumCurvature;
typedef Scalar MinimumCurvature;



/*
* Return the Gaussian Curvature for the given point v and its one-ring.
* The Gaussian Curvature is defined as:
*       1/area * ( 2Pi - sum( theta_i ) )
* where theta_i is the angle at v in the i-th face of its one-ring
*
* The definition was taken from:
* "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
* [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H. Barr ]
* Visualization and Mathematics III 2003
* Chapter 4, Paragraph 4.2, Page 12
*/
Scalar gaussianCurvature( const Vector3& v, const VectorArray< Vector3 >& p, const Scalar& area );
void   gaussianCurvature( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, const AreaMatrix& A, VectorArray< Scalar >& K );



/*
* Return the Gaussian Curvature from the maximum curvature k1 and the minimum curvature k2.
* The Gaussian Curvature is defined as:
*       K = k1 * k2
*
* This is the mathematical definition of Gaussian Curvature.
*/
Scalar gaussianCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 );
void   gaussianCurvature( const VectorArray< MaximumCurvature >& k1, const VectorArray< MinimumCurvature >& k2, VectorArray< Scalar >& K );



/*
* Return the Mean Curvature Normal from the given Laplacian vector and the given area.
* The Mean Curvature Normal is defined as:
*       Hn = 1/area * L
*
* The definition was taken from:
* "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
* [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H. Barr ]
* Visualization and Mathematics III 2003
* Chapter 3, Paragraph 3.5, Page 10
*/
Vector3 meanCurvatureNormal( const Vector3& laplacian, const Scalar& area );
void    meanCurvatureNormal( const VectorArray< Vector3 >& laplacian, const AreaMatrix& A, VectorArray< Vector3 >& Hn );



/*
* Return the Mean Curvature value from the given Mean Curvature Normal.
* The Mean Curvature is defined as:
*       H = 1/2 * || Hn ||
*
* The definition was taken from:
* "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
* [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H. Barr ]
* Visualization and Mathematics III 2003
* Chapter 3, Paragraph 3.5, Page 10
*/
Scalar meanCurvature( const Vector3& mean_curvature_normal );
void   meanCurvature( const VectorArray< Vector3 >& mean_curvature_normal, VectorArray< Scalar >& H );



/*
* Return the Mean Curvature from the maximum curvature k1 and the minimum curvature k2.
* The Mean Curvature is defined as:
*       H = 1/2 * ( k1 + k2 )
*
* This is the mathematical definition of Mean Curvature.
*/
Scalar meanCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 );
void   meanCurvature( const VectorArray< MaximumCurvature >& k1, const VectorArray< MinimumCurvature >& k2, VectorArray< Scalar >& H );


/*
* Return the Maximum Curvature from the Mean Curvature H and the Gaussian Curvature K.
* The Maximum Curvature is defined as:
*       k1 = H + sqrt( H^2 - K )
*
* This is the mathematical definition of Maximum Curvature.
*/
Scalar maxCurvature( const MeanCurvature& H, const GaussianCurvature& K );
void   maxCurvature( const VectorArray< MeanCurvature >& H, const VectorArray< GaussianCurvature >& K, VectorArray< Scalar >& k1 );



/*
* Return the Minimum Curvature from the Mean Curvature H and the Gaussian Curvature K.
* The Minimum Curvature is defined as:
*       k2 = H - sqrt( H^2 - K )
*
* This is the mathematical definition of Minimum Curvature.
*/
Scalar minCurvature( const MeanCurvature& H, const GaussianCurvature& K );
void   minCurvature( const VectorArray< MeanCurvature >& H, const VectorArray< GaussianCurvature >& K, VectorArray< Scalar >& k2 );



}
}
}

#endif // CURVATURE_DEFINITION
