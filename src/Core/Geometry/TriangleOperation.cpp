#include <Core/Geometry/TriangleOperation.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

bool isTriangleObtuse( const Math::Vector3& p, const Math::Vector3& q, const Math::Vector3& r ) {
    const Scalar cosP = ( ( q - p ).normalized() ).dot( ( r - p ).normalized() );
    const Scalar cosQ = ( ( p - q ).normalized() ).dot( ( r - q ).normalized() );
    const Scalar cosR = ( ( p - r ).normalized() ).dot( ( q - r ).normalized() );
    return ( ( cosP * cosQ * cosR ) < 0.0 );
}

Math::Vector3 triangleBarycenter( const Math::Vector3& p, const Math::Vector3& q, const Math::Vector3& r ) {
    return ( ( p + q + r ) / 3.0 );
}

Scalar triangleArea( const Math::Vector3& p, const Math::Vector3& q, const Math::Vector3& r ) {
    return ( ( ( q - p ).cross( r - p ) ).norm() * 0.5 );
}

Math::Vector3 triangleNormal( const Math::Vector3& p, const Math::Vector3& q, const Math::Vector3& r ) {
    const Math::Vector3 n = ( q - p ).cross( r - p );
    if ( n.isApprox( Math::Vector3::Zero() ) )
    {
        return Math::Vector3::Zero();
    }
    return ( n.normalized() );
}

Scalar triangleMinAngle( const Math::Vector3& p, const Math::Vector3& q, const Math::Vector3& r ) {
    const Scalar alphaP = Math::Vector::angle<Math::Vector3>( ( p - q ), ( p - r ) );
    const Scalar alphaQ = Math::Vector::angle<Math::Vector3>( ( q - p ), ( q - r ) );
    const Scalar alphaR = Math::Vector::angle<Math::Vector3>( ( r - p ), ( r - q ) );
    return std::min( alphaP, std::min( alphaQ, alphaR ) );
}

Scalar triangleMaxAngle( const Math::Vector3& p, const Math::Vector3& q, const Math::Vector3& r ) {
    const Scalar alphaP = Math::Vector::angle<Math::Vector3>( ( p - q ), ( p - r ) );
    const Scalar alphaQ = Math::Vector::angle<Math::Vector3>( ( q - p ), ( q - r ) );
    const Scalar alphaR = Math::Vector::angle<Math::Vector3>( ( r - p ), ( r - q ) );
    return std::max( alphaP, std::max( alphaQ, alphaR ) );
}

Scalar triangleAngleRatio( const Math::Vector3& p, const Math::Vector3& q, const Math::Vector3& r ) {
    const Scalar alphaP = Math::Vector::angle<Math::Vector3>( ( p - q ), ( p - r ) );
    const Scalar alphaQ = Math::Vector::angle<Math::Vector3>( ( q - p ), ( q - r ) );
    const Scalar alphaR = Math::Vector::angle<Math::Vector3>( ( r - p ), ( r - q ) );
    const Scalar min = std::min( alphaP, std::min( alphaQ, alphaR ) );
    const Scalar max = std::max( alphaP, std::max( alphaQ, alphaR ) );
    return ( min / max );
}

Math::Vector3 barycentricCoordinate( const Math::Vector3& v, const Math::Vector3& p, const Math::Vector3& q,
                               const Math::Vector3& r ) {
    Math::Vector3 result;
    const Math::Vector3 v0 = q - p;
    const Math::Vector3 v1 = r - p;
    const Math::Vector3 v2 = v - p;
    const Scalar d00 = v0.dot( v0 );
    const Scalar d01 = v0.dot( v1 );
    const Scalar d11 = v1.dot( v1 );
    const Scalar d20 = v2.dot( v0 );
    const Scalar d21 = v2.dot( v1 );
    const Scalar denom = ( d00 * d11 ) - ( d01 * d01 );
    result[1] = ( ( d11 * d20 ) - ( d01 * d21 ) ) / denom;
    result[2] = ( ( d00 * d21 ) - ( d01 * d20 ) ) / denom;
    result[0] = 1.0f - result[1] - result[2];
    return result;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
