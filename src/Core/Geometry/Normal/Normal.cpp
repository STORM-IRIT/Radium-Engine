#include <Core/Geometry/Normal/Normal.hpp>

#include <Core/Index/CircularIndex.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

//////////////
/// GLOBAL ///
//////////////

void uniformNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Vector3::Zero() );
    for( const auto& t : T ) {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        const Vector3 triN = triangleNormal( p[i], p[j], p[k] );
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }
    for( auto& n : normal ) {
        n.normalize();
    }
}



void angleWeightedNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Vector3::Zero() );
    for( const auto& t : T ) {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        const Vector3 triN = triangleNormal( p[i], p[j], p[k] );
        const Scalar theta_i = Vector::angle( ( p[j] - p[i] ), ( p[k] - p[i] ) );
        const Scalar theta_j = Vector::angle( ( p[i] - p[j] ), ( p[k] - p[j] ) );
        const Scalar theta_k = Vector::angle( ( p[i] - p[k] ), ( p[j] - p[k] ) );
        normal[i] += theta_i * triN;
        normal[j] += theta_j * triN;
        normal[k] += theta_k * triN;
    }
    for( auto& n : normal ) {
        n.normalize();
    }
}



void areaWeightedNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Vector3::Zero() );
    for( const auto& t : T ) {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        const Scalar  area = triangleArea( p[i], p[j], p[k] );
        const Vector3 triN = area * triangleNormal( p[i], p[j], p[k] );
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }
    for( auto& n : normal ) {
        n.normalize();
    }
}




////////////////
/// ONE RING ///
////////////////

Vector3 uniformNormal( const Vector3& v, const VectorArray< Vector3 >& one_ring ) {
    Vector3 normal;
    normal.setZero();
    uint N = one_ring.size();
    CircularIndex i;
    i.setSize( N );
    for( uint j = 0; j < N; ++j ) {
        i.setValue( j );
        normal += triangleNormal( v, one_ring[i], one_ring[i-1] );
    }
    return normal.normalized();
}



Vector3 angleWeightedNormal( const Vector3& v, const VectorArray< Vector3 >& one_ring ) {
    Vector3 normal;
    normal.setZero();
    uint N = one_ring.size();
    CircularIndex i;
    i.setSize( N );
    for( uint j = 0; j < N; ++j ) {
        i.setValue( j );
        Scalar theta = Vector::angle( ( one_ring[i] - v ), ( one_ring[i-1] - v ) );
        normal += theta * triangleNormal( v, one_ring[i], one_ring[i-1] );
    }
    return normal.normalized();
}



Vector3 areaWeightedNormal( const Vector3& v, const VectorArray< Vector3 >& one_ring ) {
    Vector3 normal;
    normal.setZero();
    uint N = one_ring.size();
    CircularIndex i;
    i.setSize( N );
    for( uint j = 0; j < N; ++j ) {
        i.setValue( j );
        Scalar area = triangleArea( v, one_ring[i], one_ring[i-1] );
        normal += area * triangleNormal( v, one_ring[i], one_ring[i-1] );
    }
    return normal.normalized();
}



}
}
}
