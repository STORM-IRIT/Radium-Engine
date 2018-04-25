#ifndef DISTANCE_OPERATION
#define DISTANCE_OPERATION

#include <Core/Container/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

// get distance between two sets of vertices.
void vertexDistance( const Container::VectorArray<Math::Vector3>& v0, const Container::VectorArray<Math::Vector3>& v1,
                     std::vector<Scalar>& sqrDist, Scalar& sqrMin, Scalar& sqrMax, Scalar& sqrAvg );

void vertexDistance( const Container::VectorArray<Math::Vector3>& v0, const Container::VectorArray<Math::Vector3>& v1, Scalar& sqrMin,
                     Scalar& sqrMax, Scalar& sqrAvg );

Scalar vertexDistance( const Container::VectorArray<Math::Vector3>& v0, const Container::VectorArray<Math::Vector3>& v1 );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // DISTANCE_OPERATION
