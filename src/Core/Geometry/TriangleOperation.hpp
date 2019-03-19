#ifndef TRIANGLE_OPERATION
#define TRIANGLE_OPERATION

#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// \name Triangle operations
/// \{

/**
 * Return true if the triangle is obtuse.
 */
RA_CORE_API bool isTriangleObtuse( const Vector3& p, const Vector3& q, const Vector3& r );

/**
 * Return the barycenter of the triangle PQR, computed as:
 *       ( P + Q + R ) / 3
 */
RA_CORE_API Vector3 triangleBarycenter( const Vector3& p, const Vector3& q, const Vector3& r );

/**
 * Return the area of the triangle PQR, computed as:
 *       \f[ \frac{ \| ( Q - P ) \times ( R - P ) \| }{ 2 } \f]
 */
RA_CORE_API Scalar triangleArea( const Vector3& p, const Vector3& q, const Vector3& r );

/**
 * Return the normalized normal of the triangle PQR, computed as:
 *        \f[ \frac{ ( Q - P ) \times ( R - P ) }{ \|( Q - P ) \times ( R - P )\| } \f]
 */
RA_CORE_API Vector3 triangleNormal( const Vector3& p, const Vector3& q, const Vector3& r );

/**
 * Return the value of the minimum angle of the triangle PQR.
 */
RA_CORE_API Scalar triangleMinAngle( const Vector3& p, const Vector3& q, const Vector3& r );

/**
 * Return the value of the maximum angle of the triangle PQR.
 */
RA_CORE_API Scalar triangleMaxAngle( const Vector3& p, const Vector3& q, const Vector3& r );

/**
 * Return the ratio between the minimum and the maximum angle of the triangle PQR.
 */
RA_CORE_API Scalar triangleAngleRatio( const Vector3& p, const Vector3& q, const Vector3& r );

/**
 * Return the barycentric coordinates (u, v, w) of the given point v w.r.t.\ the given triangle PQR.
 */
RA_CORE_API Vector3 barycentricCoordinate( const Vector3& v, const Vector3& p, const Vector3& q,
                                           const Vector3& r );
/// \}

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // TRIANGLE_OPERATION
