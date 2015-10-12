#include "TriangleOperation.hpp"

namespace Ra {
namespace Core {
namespace Geometry {

/*
* Return true if the triangle is obtuse.
*/
bool isTriangleObtuse( const Vector3& p, const Vector3& q, const Vector3& r ) {
    Scalar cosP = ( ( q - p ).normalized() ).dot( ( r - p ).normalized() );
    Scalar cosQ = ( ( p - q ).normalized() ).dot( ( r - q ).normalized() );
    Scalar cosR = ( ( p - r ).normalized() ).dot( ( q - r ).normalized() );
    return ( ( cosP * cosQ * cosR ) < 0.0 );
}

/*
* Return the area of the triangle PQR
*
* The area is computed as:
*       ||( Q - P ) X ( R - P )|| / 2
*/
Scalar triangleArea( const Vector3& p, const Vector3& q, const Vector3& r ) {
    return ( ( ( q - p ).cross( r - p ) ).norm() * 0.5 );
}


/*
* Return the normalized normal of the triangle PQR
*
* The normal is computed as:
*       ( Q - P ) X ( R - P ) / ||( Q - P ) X ( R - P )||
*/
Vector3 triangleNormal( const Vector3& p, const Vector3& q, const Vector3& r ) {
    return ( ( ( q - p ).cross( r - p ) ).normalized() );
}



}
}
}

