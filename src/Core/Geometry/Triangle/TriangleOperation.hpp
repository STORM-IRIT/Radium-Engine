#ifndef TRIANGLE_OPERATION
#define TRIANGLE_OPERATION

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {



/*
* Return true if the triangle is obtuse.
*/
bool isTriangleObtuse( const Vector3& p, const Vector3& q, const Vector3& r );



/*
* Return the area of the triangle PQR
*
* The area is computed as:
*       ||( Q - P ) X ( R - P )|| / 2
*/
Scalar triangleArea( const Vector3& p, const Vector3& q, const Vector3& r );



/*
* Return the normalized normal of the triangle PQR
*
* The normal is computed as:
*       ( Q - P ) X ( R - P ) / ||( Q - P ) X ( R - P )||
*/
Vector3 triangleNormal( const Vector3& p, const Vector3& q, const Vector3& r );



/*
* Return the value of the minimum angle of the triangle PQR
*/
Scalar triangleMinAngle( const Vector3& p, const Vector3& q, const Vector3& r );



/*
* Return the value of the maximum angle of the triangle PQR
*/
Scalar triangleMaxAngle( const Vector3& p, const Vector3& q, const Vector3& r );



/*
* Return the ratio between the minimum and the maximum angle of the triangle PQR
*/
Scalar triangleAngleRatio( const Vector3& p, const Vector3& q, const Vector3& r );


}
}
}

#endif // TRIANGLE_OPERATION
