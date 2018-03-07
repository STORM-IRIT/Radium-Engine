#ifndef TRIANGLE_OPERATION
#define TRIANGLE_OPERATION

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/*
 * Return true if the triangle is obtuse.
 */
RA_CORE_API bool isTriangleObtuse( const Vector3& p, const Vector3& q, const Vector3& r );

/*
 * Return the barycenter of the triangle PQR
 *
 * The barycenter is computed as:
 *       ( P + Q + R ) / 3
 */
RA_CORE_API Vector3 triangleBarycenter( const Vector3& p, const Vector3& q, const Vector3& r );

/*
 * Return the area of the triangle PQR
 *
 * The area is computed as:
 *       ||( Q - P ) X ( R - P )|| / 2
 */
RA_CORE_API Scalar triangleArea( const Vector3& p, const Vector3& q, const Vector3& r );

/*
 * Return the normalized normal of the triangle PQR
 *
 * The normal is computed as:
 *       ( Q - P ) X ( R - P ) / ||( Q - P ) X ( R - P )||
 */
RA_CORE_API Vector3 triangleNormal( const Vector3& p, const Vector3& q, const Vector3& r );

/*
 * Return the value of the minimum angle of the triangle PQR
 */
RA_CORE_API Scalar triangleMinAngle( const Vector3& p, const Vector3& q, const Vector3& r );

/*
 * Return the value of the maximum angle of the triangle PQR
 */
RA_CORE_API Scalar triangleMaxAngle( const Vector3& p, const Vector3& q, const Vector3& r );

/*
 * Return the ratio between the minimum and the maximum angle of the triangle PQR
 */
RA_CORE_API Scalar triangleAngleRatio( const Vector3& p, const Vector3& q, const Vector3& r );

/*
 * Return the barycentric coordinates (u, v, w) of the given point v for the given triangle PQR
 */
RA_CORE_API Vector3 barycentricCoordinate( const Vector3& v, const Vector3& p, const Vector3& q,
                                           const Vector3& r );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // TRIANGLE_OPERATION
