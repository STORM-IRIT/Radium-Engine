#include <Core/Geometry/Mapping/MappingOperation.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>
#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

bool isAllFinite( const Parametrization& param ) {
    const uint size = param.size();
    bool status = true;
    for ( uint i = 0; i < size; ++i )
    {
        if ( !param.at( i ).isFinite() )
        {
#ifdef CORE_DEBUG
            LOG( logWARNING ) << "Element " << i << " not finite.";
            print( param.at( i ) );
#endif
            status = false;
        }
    }
    return status;
}

bool isAllInside( const Parametrization& param ) {
    const uint size = param.size();
    bool status = true;
    for ( uint i = 0; i < size; ++i )
    {
        if ( !param.at( i ).isInside() )
        {
#ifdef CORE_DEBUG
            LOG( logWARNING ) << "Element " << i << " not inside.";
            print( param.at( i ) );
#endif
            status = false;
        }
    }
    return status;
}

bool isAllBoundToElement( const Parametrization& param ) {
    const uint size = param.size();
    bool status = true;
    for ( uint i = 0; i < size; ++i )
    {
        if ( !param.at( i ).isBoundToElement() )
        {
#ifdef CORE_DEBUG
            LOG( logWARNING ) << "Element " << i << " not bound.";
#endif
            status = false;
        }
    }
    return status;
}

void findParametrization( const TriangleMesh& source, const TriangleMesh& target,
                          Parametrization& param ) {
    const uint size = source.vertices().size();
    param.clear();
    param.resize( size );
#if defined( CORE_USE_OMP )
#    pragma omp parallel for
#endif
    for ( uint i = 0; i < size; ++i )
    {
        const Vector3& v = source.vertices()[i];
        Mapping map;
        for ( uint t = 0; t < target.m_triangles.size(); ++t )
        {
            const Vector3& t0 = target.vertices()[target.m_triangles[t][0]];
            const Vector3& t1 = target.vertices()[target.m_triangles[t][1]];
            const Vector3& t2 = target.vertices()[target.m_triangles[t][2]];
            const Vector3 n = triangleNormal( t0, t1, t2 );
            const Plane3 plane( n, t0 );
            const Vector3 p = plane.projection( v );
            const Scalar d = plane.signedDistance( v );
            const Vector3 b = barycentricCoordinate( p, t0, t1, t2 );
            // if( ( b[0] >= 0.0 ) && ( b[1] >= 0.0 ) && ( b[2] >= 0.0 ) ) {
            if ( map.isBoundToElement() )
            {
                if ( std::abs( d ) < std::abs( map.getDelta() ) )
                {
                    map.setID( Index( t ) );
                    map.setDelta( d );
                    map.setAlpha( b[0] );
                    map.setBeta( b[1] );
                }
            } else
            {
                map.setID( Index( t ) );
                map.setDelta( d );
                map.setAlpha( b[0] );
                map.setBeta( b[1] );
            }
            //}
        }
        param[i] = map;
    }
}

void applyParametrization( const TriangleMesh& inMesh, const Parametrization& param,
                           Vector3Array& outPoint, const bool FORCE_DISPLACEMENT_TO_ZERO ) {
    const uint size = param.size();
    outPoint.resize( size, Vector3::Zero() );
#if defined( CORE_USE_OMP )
#    pragma omp parallel for
#endif
    for ( uint v = 0; v < size; ++v )
    {
        const Mapping map = param[v];
        // const Scalar  alpha = map.getAlpha();
        // const Scalar  beta  = map.getBeta();
        // const Scalar  gamma = map.getGamma();
        const uint t = map.getID();
        const uint i = inMesh.m_triangles[t][0];
        const uint j = inMesh.m_triangles[t][1];
        const uint k = inMesh.m_triangles[t][2];
        const Vector3 p0 = inMesh.vertices()[i];
        const Vector3 p1 = inMesh.vertices()[j];
        const Vector3 p2 = inMesh.vertices()[k];
        // const Vector3 n0    = inMesh.normals()[i];
        // const Vector3 n1    = inMesh.normals()[j];
        // const Vector3 n2    = inMesh.normals()[k];
        const Vector3 n =
            triangleNormal( p0, p1, p2 ); //( alpha * n0 ) + ( beta * n1 ) + ( gamma * n2 );
        const Vector3 N = ( FORCE_DISPLACEMENT_TO_ZERO ) ? Vector3::Zero() : n;
        outPoint[v] = map.getPoint( p0, p1, p2, N );
    }
}

void print( const Mapping& map ) {
    LOG( logINFO ) << "Alpha : " << map.getAlpha();
    LOG( logINFO ) << "Beta  : " << map.getBeta();
    LOG( logINFO ) << "Gamma : " << map.getGamma();
    LOG( logINFO ) << "Delta : " << map.getDelta();
    LOG( logINFO ) << "ID    : " << map.getID().getValue();
}

void print( const Parametrization& param ) {
    const uint size = param.size();
    for ( uint i = 0; i < size; ++i )
    {
        print( param.at( i ) );
    }
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
