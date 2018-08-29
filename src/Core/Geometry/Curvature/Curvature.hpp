#ifndef CURVATURE_DEFINITION
#define CURVATURE_DEFINITION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Area/Area.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

using MeanCurvature = Scalar;
using GaussianCurvature = Scalar;
using MaximumCurvature = Scalar;
using MinimumCurvature = Scalar;

/**
 * Return the Gaussian Curvature for the given point \p v and its one-ring \p p.
 * The Gaussian Curvature is defined as:
 *   \f[ \frac{ 1 }{ area } * ( 2\pi - \sum \theta_i ) \f]
 *
 * where \f$ \theta_i \f$ is the angle at \p v in the i-th face of its one-ring.
 *
 * The definition was taken from:
 * "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
 * [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H. Barr ]
 * Visualization and Mathematics III 2003
 * Chapter 4, Paragraph 4.2, Page 12
 */
Scalar gaussianCurvature( const Vector3& v, const Vector3Array& p, const Scalar& area );

/**
 * Output the Gaussian Curvature for all the given points in \p according to the
 * mesh triangles in \p T and the AreaMatrix \p A.
 */
void gaussianCurvature( const Vector3Array& p, const VectorArray<Triangle>& T, const AreaMatrix& A,
                        Vector1Array& K );

/**
 * Return the Gaussian Curvature from the maximum curvature k1 and the minimum curvature k2.
 * The Gaussian Curvature is defined as:
 *   \f$ K = k1 * k2 \f$
 *
 * This is the mathematical definition of Gaussian Curvature.
 */
Scalar gaussianCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 );

/**
 * Output the Gaussian Curvature from all the given pairs of minimum and maximum curvatures
 * given in \p k1 and \p k2 respectively.
 */
void gaussianCurvature( const VectorArray<MaximumCurvature>& k1,
                        const VectorArray<MinimumCurvature>& k2, Vector1Array& K );

/**
 * Return the Mean Curvature Normal from the given Laplacian vector and the given area.
 * The Mean Curvature Normal is defined as:
 *   \f[ Hn = \frac{ 1 }{ area } * L \f]
 *
 * The definition was taken from:
 * "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
 * [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H. Barr ]
 * Visualization and Mathematics III 2003
 * Chapter 3, Paragraph 3.5, Page 10
 */
Vector3 meanCurvatureNormal( const Vector3& laplacian, const Scalar& area );

/**
 * Output the Mean Curvature Normal for each Laplacian vector given in \p laplacian
 * according to the AreaMatrix \p A.
 */
void meanCurvatureNormal( const Vector3Array& laplacian, const AreaMatrix& A, Vector3Array& Hn );

/**
 * Return the Mean Curvature value from the given Mean Curvature Normal.
 * The Mean Curvature is defined as:
 *   \f[ H = \frac{ \| Hn \| }{ 2 } \f]
 *
 * The definition was taken from:
 * "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
 * [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H. Barr ]
 * Visualization and Mathematics III 2003
 * Chapter 3, Paragraph 3.5, Page 10
 */
Scalar meanCurvature( const Vector3& mean_curvature_normal );

/**
 * Output the Mean Curvature values for each Mean Curvature Normal given in \p
 * mean_curvature_normal.
 */
void meanCurvature( const Vector3Array& mean_curvature_normal, Vector1Array& H );

/**
 * Return the Mean Curvature from the maximum curvature k1 and the minimum curvature k2.
 * The Mean Curvature is defined as:
 *   \f[ H = \frac{ k1 + k2 }{ 2 } \f]
 *
 * This is the mathematical definition of Mean Curvature.
 */
Scalar meanCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 );

/**
 * Output the Mean Curvature for each pair of maximum and minimum curvatures
 * given in \p k1 and \p k2 respectively.
 */
void meanCurvature( const VectorArray<MaximumCurvature>& k1,
                    const VectorArray<MinimumCurvature>& k2, Vector1Array& H );

/**
 * Return the Maximum Curvature from the Mean Curvature H and the Gaussian Curvature K.
 * The Maximum Curvature is defined as:
 *   \f$ k1 = H + \sqrt{ H^2 - K } \f$
 *
 * This is the mathematical definition of Maximum Curvature.
 */
Scalar maxCurvature( const MeanCurvature& H, const GaussianCurvature& K );

/**
 * Output the Maximum Curvature for each pair of Mean curvature and Gaussian Curvature
 * given in \p H and \p K respectively.
 */
void maxCurvature( const VectorArray<MeanCurvature>& H, const VectorArray<GaussianCurvature>& K,
                   Vector1Array& k1 );

/**
 * Return the Minimum Curvature from the Mean Curvature H and the Gaussian Curvature K.
 * The Minimum Curvature is defined as:
 *   \f$ k2 = H - \sqrt{ H^2 - K } \f$
 *
 * This is the mathematical definition of Minimum Curvature.
 */
Scalar minCurvature( const MeanCurvature& H, const GaussianCurvature& K );

/**
 * Output the MinimumCurvature for each pair of Mean Curvature and Gaussian Curvature
 * given in \p H and \p K respectively.
 */
void minCurvature( const VectorArray<MeanCurvature>& H, const VectorArray<GaussianCurvature>& K,
                   Vector1Array& k2 );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // CURVATURE_DEFINITION
