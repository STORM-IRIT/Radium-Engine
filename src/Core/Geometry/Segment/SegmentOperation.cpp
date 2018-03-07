#include <Core/Geometry/Segment/SegmentOperation.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

Vector3 projectPointOnSegment( const Vector3& p, const Vector3& A, const Vector3& B ) {
    const Vector3 Ap = p - A;
    const Vector3 dir = B - A;
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
