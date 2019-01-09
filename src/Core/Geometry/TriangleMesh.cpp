#include "TriangleMesh.hpp"
#include <Core/Geometry/RayCast.hpp>
#include <Core/Geometry/TriangleOperation.hpp> // triangleArea
#include <Core/Math/Types.hpp>

#include <array>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {

bool TriangleMesh::append( const TriangleMesh& other ) {
    // check same attributes through names
    if ( !m_vertexAttribs.hasSameAttribs( other.m_vertexAttribs ) )
        return false;

    // now we can proceed, topology first
    const std::size_t verticesBefore = vertices().size();
    const std::size_t trianglesBefore = m_triangles.size();
    const std::size_t facesBefore = m_faces.size();
    m_triangles.insert( m_triangles.end(), other.m_triangles.cbegin(), other.m_triangles.cend() );
    m_faces.insert( m_faces.end(), other.m_faces.cbegin(), other.m_faces.cend() );
    // Offset the vertex indices in the triangles and faces
    for ( size_t t = trianglesBefore; t < m_triangles.size(); ++t )
    {
        for ( uint i = 0; i < 3; ++i )
        {
            m_triangles[t][i] += verticesBefore;
        }
    }
    for ( size_t f = facesBefore; f < m_faces.size(); ++f )
    {
        for ( uint i = 0; i < m_faces[f].size(); ++i )
        {
            m_faces[f][i] += verticesBefore;
        }
    }

    // Deal with all attributes the same way (vertices and normals too)
    other.m_vertexAttribs.for_each_attrib( [this]( const auto& attr ) {
        if ( attr->isFloat() )
            this->append_attrib<float>( attr );
        if ( attr->isVec2() )
            this->append_attrib<Vector2>( attr );
        if ( attr->isVec3() )
            this->append_attrib<Vector3>( attr );
        if ( attr->isVec4() )
            this->append_attrib<Vector4>( attr );
    } );

    return true;
}

void TriangleMesh::clearAttributes() {
    Utils::Attrib<Vector3>::Container v;
    Utils::Attrib<Vector3>::Container n;
    std::exchange( v, vertices() );
    std::exchange( n, normals() );
    m_vertexAttribs.clear();
    initDefaultAttribs();
    std::exchange( vertices(), v );
    std::exchange( normals(), n );
}

void TriangleMesh::checkConsistency() const {
#ifdef CORE_DEBUG
    const auto nbVertices = vertices().size();
    std::vector<bool> visited( nbVertices, false );
    for ( uint t = 0; t < m_triangles.size(); ++t )
    {
        const Vector3ui& tri = m_triangles[t];
        for ( uint i = 0; i < 3; ++i )
        {
            CORE_ASSERT( uint( tri[i] ) < nbVertices, "Vertex " << tri[i] << " is in triangle " << t
                                                                << " (#" << i
                                                                << ") is out of bounds" );
            visited[tri[i]] = true;
        }
        CORE_WARN_IF( !( Geometry::triangleArea( vertices()[tri[0]], vertices()[tri[1]],
                                                 vertices()[tri[2]] ) > 0.f ),
                      "triangle " << t << " is degenerate" );
    }

    for ( uint v = 0; v < nbVertices; ++v )
    {
        CORE_ASSERT( visited[v], "Vertex " << v << " does not belong to any triangle" );
    }

    // Always have the same number of vertex data and vertices
    CORE_ASSERT( normals().size() == normals().size(), "Inconsistent number of normals" );
#endif
}

TriangleMesh::RayCastResult
TriangleMesh::castRay( const Eigen::ParametrizedLine<Scalar, 3>& ray ) const {
    RayCastResult result;

    // point cloud: get closest point
    if ( m_triangles.empty() )
    {

        Scalar minSqAngDist = std::numeric_limits<Scalar>::max();
        for ( uint i = 0; i < vertices().size(); ++i )
        {
            Scalar dist = ray.squaredDistance( vertices()[i] );

            if ( dist < minSqAngDist )
            {
                minSqAngDist = dist;
                result.m_nearestVertex = int( i );
            }
        }
        if ( result.m_nearestVertex != -1 )
        {
            result.m_t = ray.distance( vertices()[result.m_nearestVertex] );
        }
    } else
    {
        Scalar minT = std::numeric_limits<Scalar>::max();
        std::vector<Scalar> tValues;
        for ( uint i = 0; i < m_triangles.size(); ++i )
        {
            tValues.clear();
            const auto& tri = m_triangles[i];
            if ( RayCastTriangle( ray, vertices()[tri[0]], vertices()[tri[1]], vertices()[tri[2]],
                                  tValues ) &&
                 tValues[0] < minT )
            {
                minT = tValues[0];
                result.m_hitTriangle = int( i );
            }
        }

        if ( result.m_hitTriangle >= 0 )
        {
            result.m_t = minT;
            Scalar minDist = std::numeric_limits<Scalar>::max();
            const Vector3ui& T = m_triangles[result.m_hitTriangle];
            std::array<Vector3, 3> V = {vertices()[T[0]], vertices()[T[1]], vertices()[T[2]]};
            const Vector3 I = ray.pointAt( minT );
            // find closest vertex
            // \FIXME Investigate how to use lineToTriSq here
            for ( uint i = 0; i < 3; ++i )
            {
                Scalar dSq = ( V[i] - I ).squaredNorm();
                if ( dSq < minDist )
                {
                    result.m_nearestVertex = T( i );
                    minDist = dSq;
                }
            }
            // find closest edge vertices
            const Scalar inv_2area = Scalar( 1. ) / ( V[1] - V[0] ).cross( V[2] - V[0] ).norm();
            const Scalar u = ( V[2] - V[1] ).cross( I - V[1] ).norm() * inv_2area;
            const Scalar v = ( V[0] - V[2] ).cross( I - V[2] ).norm() * inv_2area;
            const Scalar w = Scalar( 1. ) - u - v;
            if ( u < v && u < w )
            {
                result.m_edgeVertex0 = T( 1 );
                result.m_edgeVertex1 = T( 2 );
            } else if ( v < w )
            {
                result.m_edgeVertex0 = T( 0 );
                result.m_edgeVertex1 = T( 2 );
            } else
            {
                result.m_edgeVertex0 = T( 0 );
                result.m_edgeVertex1 = T( 1 );
            }
        }
    }

    return result;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
