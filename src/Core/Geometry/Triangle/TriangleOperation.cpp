#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

bool isTriangleObtuse( const Vector3& p, const Vector3& q, const Vector3& r ) {
    const Scalar cosP = ( ( q - p ).normalized() ).dot( ( r - p ).normalized() );
    const Scalar cosQ = ( ( p - q ).normalized() ).dot( ( r - q ).normalized() );
    const Scalar cosR = ( ( p - r ).normalized() ).dot( ( q - r ).normalized() );
    return ( ( cosP * cosQ * cosR ) < 0.0 );
}



Scalar triangleArea( const Vector3& p, const Vector3& q, const Vector3& r ) {
    return ( ( ( q - p ).cross( r - p ) ).norm() * 0.5 );
}



Vector3 triangleNormal( const Vector3& p, const Vector3& q, const Vector3& r ) {
    const Vector3 n = ( q - p ).cross( r - p );
    if( n.isApprox( Vector3::Zero() ) ) {
        return Vector3::Zero();
    }
    return ( n.normalized() );
}



Scalar triangleMinAngle( const Vector3& p, const Vector3& q, const Vector3& r ) {
    const Scalar alphaP = Vector::angle< Vector3 >( ( p - q ), ( p - r ) );
    const Scalar alphaQ = Vector::angle< Vector3 >( ( q - p ), ( q - r ) );
    const Scalar alphaR = Vector::angle< Vector3 >( ( r - p ), ( r - q ) );
    return std::min( alphaP, std::min( alphaQ, alphaR ) );
}



Scalar triangleMaxAngle( const Vector3& p, const Vector3& q, const Vector3& r ) {
    const Scalar alphaP = Vector::angle< Vector3 >( ( p - q ), ( p - r ) );
    const Scalar alphaQ = Vector::angle< Vector3 >( ( q - p ), ( q - r ) );
    const Scalar alphaR = Vector::angle< Vector3 >( ( r - p ), ( r - q ) );
    return std::max( alphaP, std::max( alphaQ, alphaR ) );
}



Scalar triangleAngleRatio( const Vector3& p, const Vector3& q, const Vector3& r ) {
    const Scalar alphaP = Vector::angle< Vector3 >( ( p - q ), ( p - r ) );
    const Scalar alphaQ = Vector::angle< Vector3 >( ( q - p ), ( q - r ) );
    const Scalar alphaR = Vector::angle< Vector3 >( ( r - p ), ( r - q ) );
    const Scalar min = std::min( alphaP, std::min( alphaQ, alphaR ) );
    const Scalar max = std::max( alphaP, std::max( alphaQ, alphaR ) );
    return ( min / max );
}


}
}
}

