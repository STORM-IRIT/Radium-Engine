#ifndef SEGMENT_OPERATION
#define SEGMENT_OPERATION

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

Vector3 projectPointOnSegment( const Vector3& p, const Vector3& A, const Vector3& B );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // SEGMENT_OPERATION
