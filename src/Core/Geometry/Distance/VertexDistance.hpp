#ifndef DISTANCE_OPERATION
#define DISTANCE_OPERATION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// Output the list of squared pairwise distances, along with the squared
/// mininal, maximal and average distances, between two sets of vertices.
void vertexDistance( const Vector3Array& v0, const Vector3Array& v1, std::vector<Scalar>& sqrDist,
                     Scalar& sqrMin, Scalar& sqrMax, Scalar& sqrAvg );

/// Output the squared minimal, maximal and average pairwise distances between two sets of vertices.
void vertexDistance( const Vector3Array& v0, const Vector3Array& v1, Scalar& sqrMin, Scalar& sqrMax,
                     Scalar& sqrAvg );

/// Return the squared average pairwise distance between two sets of vertices.
Scalar vertexDistance( const VectorArray<Vector3>& v0, const VectorArray<Vector3>& v1 );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // DISTANCE_OPERATION
