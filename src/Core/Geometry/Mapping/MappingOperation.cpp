#include <Core/Geometry/Mapping/MappingOperation.hpp>

#ifdef CORE_DEBUG
#include <Core/Log/Log.hpp>
#endif

namespace Ra {
namespace Core {
namespace Geometry {

bool isAllFinite( const Parametrization& param ) {
    const uint size = param.size();
    bool status = true;
    for( uint i = 0; i < size; ++i ) {
        if( !param.at( i ).isFinite() ) {
#ifdef CORE_DEBUG
            LOG( logWARNING ) << "Element " << i << " not finite.";
            print( param.at( i ) );
#endif
            status = false;
        }
    }
    return status;
}



bool isAllPositive( const Parametrization& param ) {
    const uint size = param.size();
    bool status = true;
    for( uint i = 0; i < size; ++i ) {
        if( !param.at( i ).isPositive() ) {
#ifdef CORE_DEBUG
            LOG( logWARNING ) << "Element " << i << " not positive.";
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
    for( uint i = 0; i < size; ++i ) {
        if( !param.at( i ).isBoundToElement() ) {
#ifdef CORE_DEBUG
            LOG( logWARNING ) << "Element " << i << " not bound.";
#endif
            status = false;
        }
    }
    return status;
}



void applyParametrization( const TriangleMesh& inMesh, const Parametrization& param, Vector3Array& outPoint, const bool FORCE_DISPLACEMENT_TO_ZERO ) {
    const uint size = param.size();
    outPoint.resize( size, Vector3::Zero() );
    #pragma omp parallel for
    for( uint v = 0; v < size; ++v ) {
        const Mapping map   = param[v];
        const Scalar  alpha = map.getAlpha();
        const Scalar  beta  = map.getBeta();
        const Scalar  gamma = map.getGamma();
        const uint    t     = map.getID();
        const uint    i     = inMesh.m_triangles[t][0];
        const uint    j     = inMesh.m_triangles[t][1];
        const uint    k     = inMesh.m_triangles[t][2];
        const Vector3 p0    = inMesh.m_vertices[i];
        const Vector3 p1    = inMesh.m_vertices[j];
        const Vector3 p2    = inMesh.m_vertices[k];
        const Vector3 n0    = inMesh.m_normals[i];
        const Vector3 n1    = inMesh.m_normals[j];
        const Vector3 n2    = inMesh.m_normals[k];
        const Vector3 N     = ( FORCE_DISPLACEMENT_TO_ZERO ) ? Vector3::Zero() : ( ( alpha * n0 ) + ( beta * n1 ) + ( gamma * n2 ) ).eval();
        outPoint[v] = map.getPoint( p0, p1, p2, N );
    }
}



void print( const Mapping& map ) {
#ifdef CORE_DEBUG
    LOG( logINFO ) << "Alpha : " << map.getAlpha();
    LOG( logINFO ) << "Beta  : " << map.getBeta();
    LOG( logINFO ) << "Gamma : " << map.getGamma();
    LOG( logINFO ) << "Delta : " << map.getDelta();
    LOG( logINFO ) << "ID    : " << map.getID();
#else
    std::cout << "Alpha : " << map.getAlpha() << std::endl;
    std::cout << "Beta  : " << map.getBeta() << std::endl;
    std::cout << "Gamma : " << map.getGamma() << std::endl;
    std::cout << "Delta : " << map.getDelta() << std::endl;
    std::cout << "ID    : " << map.getID().getValue() << std::endl;
#endif
}

void print( const Parametrization& param ) {
    const uint size = param.size();
    for( uint i = 0; i < size; ++i ) {
        print( param.at( i ) );
    }
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
