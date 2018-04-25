#include <Core/Geometry/MappingOperation.hpp>
#include <Core/Geometry/TriangleOperation.hpp>
#include <Core/Utils/Log.hpp>

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
            LOG( Core::Utils::logWARNING ) << "Element " << i << " not finite.";
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
            LOG( Core::Utils::logWARNING ) << "Element " << i << " not inside.";
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
            LOG( Core::Utils::logWARNING ) << "Element " << i << " not bound.";
#endif
            status = false;
        }
    }
    return status;
}

void findParametrization( const TriangleMesh& source, const TriangleMesh& target,
                          Parametrization& param ) {
    const uint size = source.m_vertices.size();
    param.clear();
    param.resize( size );
#if defined( CORE_USE_OMP )
#    pragma omp parallel for
#endif
    for ( uint i = 0; i < size; ++i )
    {
        const Math::Vector3& v = source.m_vertices[i];
        Mapping map;
        for ( uint t = 0; t < target.m_triangles.size(); ++t )
        {
            const Math::Vector3& t0 = target.m_vertices[target.m_triangles[t][0]];
            const Math::Vector3& t1 = target.m_vertices[target.m_triangles[t][1]];
            const Math::Vector3& t2 = target.m_vertices[target.m_triangles[t][2]];
            const Math::Vector3 n = triangleNormal( t0, t1, t2 );
            const Math::Plane3 plane( n, t0 );
            const Math::Vector3 p = plane.projection( v );
            const Scalar d = plane.signedDistance( v );
            const Math::Vector3 b = barycentricCoordinate( p, t0, t1, t2 );
            // if( ( b[0] >= 0.0 ) && ( b[1] >= 0.0 ) && ( b[2] >= 0.0 ) ) {
            if ( map.isBoundToElement() )
            {
                if ( std::abs( d ) < std::abs( map.getDelta() ) )
                {
                    map.setID( Container::Index( t ) );
                    map.setDelta( d );
                    map.setAlpha( b[0] );
                    map.setBeta( b[1] );
                }
            } else
            {
                map.setID( Container::Index( t ) );
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
                           Container::Vector3Array& outPoint, const bool FORCE_DISPLACEMENT_TO_ZERO ) {
    const uint size = param.size();
    outPoint.resize( size, Math::Vector3::Zero() );
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
        const Math::Vector3 p0 = inMesh.m_vertices[i];
        const Math::Vector3 p1 = inMesh.m_vertices[j];
        const Math::Vector3 p2 = inMesh.m_vertices[k];
        // const Math::Vector3 n0    = inMesh.m_normals[i];
        // const Math::Vector3 n1    = inMesh.m_normals[j];
        // const Math::Vector3 n2    = inMesh.m_normals[k];
        const Math::Vector3 n =
            triangleNormal( p0, p1, p2 ); //( alpha * n0 ) + ( beta * n1 ) + ( gamma * n2 );
        const Math::Vector3 N = ( FORCE_DISPLACEMENT_TO_ZERO ) ? Math::Vector3::Zero() : n;
        outPoint[v] = map.getPoint( p0, p1, p2, N );
    }
}

void print( const Mapping& map ) {
    LOG( Utils::logINFO ) << "Alpha : " << map.getAlpha();
    LOG( Utils::logINFO ) << "Beta  : " << map.getBeta();
    LOG( Utils::logINFO ) << "Gamma : " << map.getGamma();
    LOG( Utils::logINFO ) << "Delta : " << map.getDelta();
    LOG( Utils::logINFO ) << "ID    : " << map.getID().getValue();
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
