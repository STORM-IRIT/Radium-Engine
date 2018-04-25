#ifndef SEGMENT_OPERATION
#define SEGMENT_OPERATION

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

Math::Vector3 projectPointOnSegment( const Math::Vector3& p, const Math::Vector3& A, const Math::Vector3& B );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // SEGMENT_OPERATION
