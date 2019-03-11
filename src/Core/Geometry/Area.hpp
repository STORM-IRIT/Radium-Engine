#ifndef AREA_DEFINITION
#define AREA_DEFINITION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/** \name Area
 * The definition was taken from:
 *
 * "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
 * [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H.\ Barr ]
 * Visualization and Mathematics III 2003
 */
/// \{

/**
 * Return the area for the given point v and its one-ring.
 * The values correspond to the one-ring area of v.
 */
RA_CORE_API Scalar oneRingArea( const Vector3& v, const VectorArray<Vector3>& p );

/**
 * Return the area for the given point v and its one-ring.
 * The values correspond to the barycentric area of v.
 */
RA_CORE_API Scalar barycentricArea( const Vector3& v, const VectorArray<Vector3>& p );

/**
 * Return the area for the given point v and its one-ring.
 * The values correspond to the Voronoi area of v.
 */
RA_CORE_API Scalar voronoiArea( const Vector3& v, const VectorArray<Vector3>& p );

/**
 * Return the area for the given point v and its one-ring.
 * The values correspond to the mixed area of v.
 */
RA_CORE_API Scalar mixedArea( const Vector3& v, const VectorArray<Vector3>& p );
/// \}

/** \name Area Matrices
 * The definition was taken from:
 *
 * "Discrete Differential-Geometry Operators for Triangulated 2-Manifolds"
 * [ Mark Meyer, Mathieu Desbrun, Peter Schoder, Alan H.\ Barr ]
 * Visualization and Mathematics III 2003
 */
/// \{

/**
 * Define the AreaMatrix as the diagonal matrix such that:
 *  - A( i, j ) = area(\f$ P_i \f$) , if i == j
 *  - A( i, j ) = 0                 , if i != j
 *
 * where \f$ P_i \f$ is the i-th point of the mesh.
 */
using AreaMatrix = Diagonal;

/**
 * Return the AreaMatrix for the given set of points and triangles.
 * The values correspond to the one-ring area of each point in p.
 */
RA_CORE_API AreaMatrix oneRingArea( const VectorArray<Vector3>& p,
                                    const VectorArray<Vector3ui>& T );

/**
 * Return the AreaMatrix for the given set of points and triangles.
 * The values correspond to the one-ring area of each point in p.
 */
RA_CORE_API void oneRingArea( const VectorArray<Vector3>& p, const VectorArray<Vector3ui>& T,
                              AreaMatrix& A );

/**
 * Return the AreaMatrix for the given set of points and triangles.
 * The values correspond to the barycentric area of each point in p.
 */
RA_CORE_API AreaMatrix barycentricArea( const VectorArray<Vector3>& p,
                                        const VectorArray<Vector3ui>& T );

/**
 * Return the AreaMatrix for the given set of points and triangles.
 * The values correspond to the barycentric area of each point in p.
 */
RA_CORE_API void barycentricArea( const VectorArray<Vector3>& p, const VectorArray<Vector3ui>& T,
                                  AreaMatrix& A );

/**
 * Return the AreaMatrix for the given set of points and triangles.
 * The values correspond to the Voronoi area of each point in p.
 */
RA_CORE_API AreaMatrix voronoiArea( const VectorArray<Vector3>& p,
                                    const VectorArray<Vector3ui>& T );

/**
 * Return the AreaMatrix for the given set of points and triangles.
 * The values correspond to the mixed area of each point in p.
 */
RA_CORE_API AreaMatrix mixedArea( const VectorArray<Vector3>& p, const VectorArray<Vector3ui>& T );
/// \}

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // AREA_DEFINITION
