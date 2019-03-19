#ifndef DISTANCE_OPERATION
#define DISTANCE_OPERATION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// \name Vertices distances
/// \{
// FIXME: this should go to DistanceQueries.*

/**
 * Output the list of pairwise squared distances, along with the squared
 * mininal, maximal and average distances, between two sets of vertices.
 */
RA_CORE_API void vertexDistance( const VectorArray<Vector3>& v0, const VectorArray<Vector3>& v1,
                                 std::vector<Scalar>& sqrDist, Scalar& sqrMin, Scalar& sqrMax,
                                 Scalar& sqrAvg );

/**
 * Output the squared minimal, maximal and average pairwise distances between two sets of vertices.
 */
RA_CORE_API void vertexDistance( const VectorArray<Vector3>& v0, const VectorArray<Vector3>& v1,
                                 Scalar& sqrMin, Scalar& sqrMax, Scalar& sqrAvg );

/**
 * Return the squared average pairwise distance between two sets of vertices.
 */
RA_CORE_API Scalar vertexDistance( const VectorArray<Vector3>& v0, const VectorArray<Vector3>& v1 );
/// \}

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // DISTANCE_OPERATION
