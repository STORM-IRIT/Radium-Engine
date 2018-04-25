#include <Core/Geometry/SegmentOperation.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

Math::Vector3 projectPointOnSegment( const Math::Vector3& p, const Math::Vector3& A, const Math::Vector3& B ) {
    const Math::Vector3 Ap = p - A;
    const Math::Vector3 dir = B - A;
    // Square length of segment
    const Scalar length_sq = dir.squaredNorm();
    CORE_ASSERT( ( length_sq != 0.0 ), "The segment is a point" );
    // Project on the line segment
    const Scalar t =
        std::min( (Scalar)1.0, std::max( ( Ap.dot( dir ) / length_sq ), (Scalar)0.0 ) );
    return ( A + ( t * dir ) );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
