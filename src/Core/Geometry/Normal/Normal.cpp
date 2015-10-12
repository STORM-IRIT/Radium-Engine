#include "Normal.hpp"

#include <Core/Index/CircularIndex.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

////////////////
/// ONE RING ///
////////////////

Vector3 uniformNormal( const Vector3& v, const VectorArray< Vector3 >& p ) {
    Vector3 normal;
    normal.setZero();
    uint N = p.size();
    CircularIndex i;
    i.setSize( N );
    for( uint j = 0; j < N; ++j ) {
        i.setValue( j );
        normal += triangleNormal( v, p[i], p[i-1] );
    }
    return normal.normalized();
}



Vector3 angleWeigthedNormal( const Vector3& v, const VectorArray< Vector3 >& p ) {
    Vector3 normal;
    normal.setZero();
    uint N = p.size();
    CircularIndex i;
    i.setSize( N );
    for( uint j = 0; j < N; ++j ) {
        i.setValue( j );
        Scalar theta = Vector::angle( ( p[i] - v ), ( p[i-1] - v ) );
        normal += theta * triangleNormal( v, p[i], p[i-1] );
    }
    return normal.normalized();
}



Vector3 areaWeightedNormal( const Vector3& v, const VectorArray< Vector3 >& p ) {
    Vector3 normal;
    normal.setZero();
    uint N = p.size();
    CircularIndex i;
    i.setSize( N );
    for( uint j = 0; j < N; ++j ) {
        i.setValue( j );
        Scalar area = triangleArea( v, p[i], p[i-1] );
        normal += area * triangleNormal( v, p[i], p[i-1] );
    }
    return normal.normalized();
}



}
}
}
