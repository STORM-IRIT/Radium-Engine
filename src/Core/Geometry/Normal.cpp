#include <Core/Geometry/Normal.hpp>

#include <Core/Geometry/TriangleOperation.hpp>
#include <Core/Container/CircularIndex.hpp>

#include <Core/Utils/Timer.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

//////////////
/// GLOBAL ///
//////////////

void uniformNormal( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T,
                    Container::VectorArray<Math::Vector3>& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Math::Vector3::Zero() );

    for ( const auto& t : T )
    {
        const uint i = t( 0 );
        const uint j = t( 1 );
        const uint k = t( 2 );
        const Math::Vector3 triN = triangleNormal( p[i], p[j], p[k] );
        if ( !triN.allFinite() )
        {
            continue;
        }
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }

#pragma omp parallel for
    for ( uint i = 0; i < N; ++i )
    {
        if ( !normal[i].isApprox( Math::Vector3::Zero() ) )
        {
            normal[i].normalize();
        }
    }
}

void uniformNormal( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T,
                    const std::vector<Ra::Core::Container::Index>& duplicateTable,
                    Container::VectorArray<Math::Vector3>& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Math::Vector3::Zero() );

    for ( const auto& t : T )
    {
        const Container::Index i = duplicateTable.at( t( 0 ) );
        const Container::Index j = duplicateTable.at( t( 1 ) );
        const Container::Index k = duplicateTable.at( t( 2 ) );
        const Math::Vector3 triN = triangleNormal( p[i], p[j], p[k] );
        if ( !triN.allFinite() )
        {
            continue;
        }
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }

#pragma omp parallel for
    for ( uint i = 0; i < N; ++i )
    {
        if ( !normal[i].isApprox( Math::Vector3::Zero() ) )
        {
            normal[i].normalize();
        }
    }

#pragma omp parallel for
    for ( uint i = 0; i < N; ++i )
    {
        normal[i] = normal[duplicateTable[i]];
    }
}

Math::Vector3 localUniformNormal( const uint i, const Container::VectorArray<Math::Vector3>& p,
                            const Container::VectorArray<Triangle>& T, const TVAdj& adj ) {
    Math::Vector3 normal = Math::Vector3::Zero();
    for ( TVAdj::InnerIterator it( adj, i ); it; ++it )
    {
        const uint t = it.row();
        const uint i = T[t]( 0 );
        const uint j = T[t]( 1 );
        const uint k = T[t]( 2 );
        normal += triangleNormal( p[i], p[j], p[k] );
    }
    return normal; //.normalized();
}

void angleWeightedNormal( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T,
                          Container::VectorArray<Math::Vector3>& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Math::Vector3::Zero() );
    for ( const auto& t : T )
    {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        const Math::Vector3 triN = triangleNormal( p[i], p[j], p[k] );
        const Scalar theta_i = Math::Vector::angle( ( p[j] - p[i] ), ( p[k] - p[i] ) );
        const Scalar theta_j = Math::Vector::angle( ( p[i] - p[j] ), ( p[k] - p[j] ) );
        const Scalar theta_k = Math::Vector::angle( ( p[i] - p[k] ), ( p[j] - p[k] ) );
        normal[i] += theta_i * triN;
        normal[j] += theta_j * triN;
        normal[k] += theta_k * triN;
    }
    for ( auto& n : normal )
    {
        n.normalize();
    }
}

void areaWeightedNormal( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T,
                         Container::VectorArray<Math::Vector3>& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Math::Vector3::Zero() );
    for ( const auto& t : T )
    {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        const Scalar area = triangleArea( p[i], p[j], p[k] );
        const Math::Vector3 triN = area * triangleNormal( p[i], p[j], p[k] );
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }
    for ( auto& n : normal )
    {
        n.normalize();
    }
}

////////////////
/// ONE RING ///
////////////////

Math::Vector3 uniformNormal( const Math::Vector3& v, const Container::VectorArray<Math::Vector3>& one_ring ) {
    Math::Vector3 normal;
    normal.setZero();
    uint N = one_ring.size();
    Container::CircularIndex i;
    i.setSize( N );
    for ( uint j = 0; j < N; ++j )
    {
        i.setValue( j );
        normal += triangleNormal( v, one_ring[i], one_ring[i - 1] );
    }
    if ( !normal.isApprox( Math::Vector3::Zero() ) )
    {
        return normal.normalized();
    }
    return Math::Vector3::Zero();
}

Math::Vector3 angleWeightedNormal( const Math::Vector3& v, const Container::VectorArray<Math::Vector3>& one_ring ) {
    Math::Vector3 normal;
    normal.setZero();
    uint N = one_ring.size();
    Container::CircularIndex i;
    i.setSize( N );
    for ( uint j = 0; j < N; ++j )
    {
        i.setValue( j );
        Scalar theta = Math::Vector::angle( ( one_ring[i] - v ), ( one_ring[i - 1] - v ) );
        normal += theta * triangleNormal( v, one_ring[i], one_ring[i - 1] );
    }
    if ( !normal.isApprox( Math::Vector3::Zero() ) )
    {
        return normal.normalized();
    }
    return Math::Vector3::Zero();
}

Math::Vector3 areaWeightedNormal( const Math::Vector3& v, const Container::VectorArray<Math::Vector3>& one_ring ) {
    Math::Vector3 normal;
    normal.setZero();
    uint N = one_ring.size();
    Container::CircularIndex i;
    i.setSize( N );
    for ( uint j = 0; j < N; ++j )
    {
        i.setValue( j );
        Scalar area = triangleArea( v, one_ring[i], one_ring[i - 1] );
        normal += area * triangleNormal( v, one_ring[i], one_ring[i - 1] );
    }
    if ( !normal.isApprox( Math::Vector3::Zero() ) )
    {
        return normal.normalized();
    }
    return Math::Vector3::Zero();
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
