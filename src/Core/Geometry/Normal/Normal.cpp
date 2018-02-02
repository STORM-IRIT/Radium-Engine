#include <Core/Geometry/Normal/Normal.hpp>

#include <Core/Index/CircularIndex.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>

#include <Core/Time/Timer.hpp>

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
        const uint i = t( 0 );
        const uint j = t( 1 );
        const uint k = t( 2 );
        const Vector3 triN = triangleNormal( p[i], p[j], p[k] );
        if( !triN.allFinite() ) {
            continue;
        }
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }

    #pragma omp parallel for
    for( uint i = 0; i < N; ++i ) {
        if( !normal[i].isApprox( Vector3::Zero() ) ) {
            normal[i].normalize();
        }
    }
}


void uniformNormal( const VectorArray< Vector3 >& p,
                    const VectorArray< Triangle >& T,
                    const std::vector<Ra::Core::Index>& duplicateTable,
                    VectorArray< Vector3 >& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Vector3::Zero() );

    for( const auto& t : T ) {
        const Index i = duplicateTable.at( t( 0 ) );
        const Index j = duplicateTable.at( t( 1 ) );
        const Index k = duplicateTable.at( t( 2 ) );
        const Vector3 triN = triangleNormal( p[i], p[j], p[k] );
        if( !triN.allFinite() ) {
            continue;
        }
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }

    #pragma omp parallel for
    for( uint i = 0; i < N; ++i ) {
        if( !normal[i].isApprox( Vector3::Zero() ) ) {
            normal[i].normalize();
        }
    }

    #pragma omp parallel for
    for( uint i = 0; i < N; ++i ) {
        normal[i] = normal[ duplicateTable[i] ];
    }
}



Vector3 localUniformNormal( const uint i, const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, const TVAdj& adj ) {
    Vector3 normal = Vector3::Zero();
    for( TVAdj::InnerIterator it( adj, i ); it; ++it ) {
        const uint t = it.row();
        const uint i = T[t]( 0 );
        const uint j = T[t]( 1 );
        const uint k = T[t]( 2 );
        normal += triangleNormal( p[i], p[j], p[k] );
    }
    return normal;//.normalized();
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
    if( !normal.isApprox( Vector3::Zero() ) ) {
        return normal.normalized();
    }
    return Vector3::Zero();
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
    if( !normal.isApprox( Vector3::Zero() ) ) {
        return normal.normalized();
    }
    return Vector3::Zero();
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
    if( !normal.isApprox( Vector3::Zero() ) ) {
        return normal.normalized();
    }
    return Vector3::Zero();
}



}
}
}
