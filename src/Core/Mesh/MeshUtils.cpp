#include <Core/Mesh/MeshUtils.hpp>

#include <Core/Log/Log.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Math/RayCast.hpp>
#include <Core/String/StringUtils.hpp>

#include <map>
#include <set>
#include <utility>

#include <algorithm>
#include <utility>
#include <vector>

namespace Ra {
namespace Core {
namespace MeshUtils {

void getAutoNormals( TriangleMesh& mesh, VectorArray<Vector3>& normalsOut ) {
    const uint numVertices = mesh.vertices().size();
    const uint numTriangles = mesh.m_triangles.size();

    normalsOut.clear();
    normalsOut.resize( numVertices, Vector3::Zero() );

    for ( uint t = 0; t < numTriangles; t++ )
    {
        const Triangle& tri = mesh.m_triangles[t];
        Vector3 n = getTriangleNormal( mesh, t );

        for ( uint i = 0; i < 3; ++i )
        {
            normalsOut[tri[i]] += n;
        }
    }

    normalsOut.getMap().colwise().normalize();
}

bool findDuplicates( const TriangleMesh& mesh, std::vector<VertexIdx>& duplicatesMap ) {
    bool hasDuplicates = false;
    duplicatesMap.clear();
    const uint numVerts = mesh.vertices().size();
    duplicatesMap.resize( numVerts, VertexIdx( -1 ) );

    VectorArray<Vector3>::const_iterator vertPos;
    VectorArray<Vector3>::const_iterator duplicatePos;
    std::vector<std::pair<Vector3, VertexIdx>> vertices;

    for ( uint i = 0; i < numVerts; ++i )
    {
        vertices.push_back( std::make_pair( mesh.vertices()[i], VertexIdx( i ) ) );
    }

    std::sort( vertices.begin(), vertices.end(),
               []( std::pair<Vector3, int> a, std::pair<Vector3, int> b ) {
                   if ( a.first.x() == b.first.x() )
                   {
                       if ( a.first.y() == b.first.y() )
                           if ( a.first.z() == b.first.z() )
                               return a.second < b.second;
                           else
                               return a.first.z() < b.first.z();
                       else
                           return a.first.y() < b.first.y();
                   }
                   return a.first.x() < b.first.x();
               } );
    // Here vertices contains vertex pos and idx, with equal
    // vertices contiguous, sorted by idx, so checking if current
    // vertex equals the previous one state if its a duplicated
    // vertex position.
    duplicatesMap[vertices[0].second] = vertices[0].second;
    for ( uint i = 1; i < numVerts; ++i )
    {
        if ( vertices[i].first == vertices[i - 1].first )
        {
            duplicatesMap[vertices[i].second] = duplicatesMap[vertices[i - 1].second];
            hasDuplicates = true;
        } else
        { duplicatesMap[vertices[i].second] = vertices[i].second; }
    }

    return hasDuplicates;
}

void removeDuplicates( TriangleMesh& mesh, std::vector<VertexIdx>& vertexMap ) {
    std::vector<VertexIdx> duplicatesMap;
    findDuplicates( mesh, duplicatesMap );

    std::vector<VertexIdx> newIndices( mesh.vertices().size(), VertexIdx( -1 ) );
    Vector3Array uniqueVertices;
    for ( uint i = 0; i < mesh.vertices().size(); i++ )
    {
        if ( duplicatesMap[i] == i )
        {
            newIndices[i] = uniqueVertices.size();
            uniqueVertices.push_back( mesh.vertices()[i] );
        }
    }

    for ( uint i = 0; i < mesh.m_triangles.size(); i++ )
    {
        for ( uint j = 0; j < 3; j++ )
        {
            int oldIdx = mesh.m_triangles[i]( j );
            int newIdx = newIndices[duplicatesMap[oldIdx]];
            mesh.m_triangles[i]( j ) = newIdx;
        }
    }

    vertexMap.resize( mesh.vertices().size() );
    for ( uint i = 0; i < mesh.vertices().size(); i++ )
        vertexMap[i] = newIndices[duplicatesMap[i]];

    mesh.vertices() = uniqueVertices;
}

RayCastResult castRay( const TriangleMesh& mesh, const Ray& ray ) {
    RayCastResult result;

    // point cloud: get closest point
    if ( mesh.m_triangles.empty() )
    {

        Scalar minSqAngDist = std::numeric_limits<Scalar>::max();
        for ( uint i = 0; i < mesh.vertices().size(); ++i )
        {
            Scalar dist = ray.squaredDistance( mesh.vertices()[i] );

            if ( dist < minSqAngDist )
            {
                minSqAngDist = dist;
                result.m_nearestVertex = int( i );
            }
        }
        if ( result.m_nearestVertex != -1 )
        {
            result.m_t = ray.distance( mesh.vertices()[result.m_nearestVertex] );
        }
    } else
    {
        Scalar minT = std::numeric_limits<Scalar>::max();
        std::vector<Scalar> tValues;
        std::array<Vector3, 3> v;
        for ( uint i = 0; i < mesh.m_triangles.size(); ++i )
        {
            tValues.clear();
            getTriangleVertices( mesh, i, v );
            if ( RayCast::vsTriangle( ray, v[0], v[1], v[2], tValues ) && tValues[0] < minT )
            {
                minT = tValues[0];
                result.m_hitTriangle = int( i );
            }
        }

        if ( result.m_hitTriangle >= 0 )
        {
            result.m_t = minT;
            Scalar minDist = std::numeric_limits<Scalar>::max();
            std::array<Vector3, 3> V;
            getTriangleVertices( mesh, result.m_hitTriangle, V );
            const Triangle& T = mesh.m_triangles[result.m_hitTriangle];
            const Vector3 I = ray.pointAt( minT );
            // find closest vertex
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
            const Scalar inv_2area = 1.0 / ( V[1] - V[0] ).cross( V[2] - V[0] ).norm();
            const Scalar u = ( V[2] - V[1] ).cross( I - V[1] ).norm() * inv_2area;
            const Scalar v = ( V[0] - V[2] ).cross( I - V[2] ).norm() * inv_2area;
            const Scalar w = 1.0 - u - v;
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

/// Return the mean edge length of the given triangle mesh
Scalar getMeanEdgeLength( const TriangleMesh& mesh ) {
    using Key = std::pair<uint, uint>;
    std::set<Key> list;
    const uint size = mesh.m_triangles.size();
    uint edgeSize = 0;
    Scalar edgeLength = 0.0;
#pragma omp parallel for
    for ( int t = 0; t < int( size ); ++t )
    {
        for ( uint v = 0; v < 3; ++v )
        {
            const uint i = mesh.m_triangles[t][v];
            const uint j = mesh.m_triangles[t][( v + 1 ) % 3];
            Key k( ( ( i < j ) ? i : j ), ( ( i < j ) ? j : i ) );
            Scalar length = ( mesh.vertices()[i] - mesh.vertices()[j] ).norm();
#pragma omp critical
            {
                auto it = list.find( k );
                if ( it == list.end() )
                {
                    list.insert( k );
                    ++edgeSize;
                    edgeLength += length;
                }
            }
        }
    }
    if ( edgeSize != 0 )
    {
        return ( edgeLength / Scalar( edgeSize ) );
    }
    return 0.0;
}

void checkConsistency( const TriangleMesh& mesh ) {
#ifdef CORE_DEBUG
    std::vector<bool> visited( mesh.vertices().size(), false );
    for ( uint t = 0; t < mesh.m_triangles.size(); ++t )
    {
        CORE_WARN_IF( !( getTriangleArea( mesh, t ) > 0.f ), "Triangle " << t << " is degenerate" );
        const Triangle& tri = mesh.m_triangles[t];
        for ( uint i = 0; i < 3; ++i )
        {
            CORE_ASSERT( uint( tri[i] ) < mesh.vertices().size(),
                         "Vertex " << tri[i] << " is in triangle " << t << " (#" << i
                                   << ") is out of bounds" );
            visited[tri[i]] = true;
        }
    }

    for ( uint v = 0; v < visited.size(); ++v )
    {
        CORE_ASSERT( visited[v], "Vertex " << v << " does not belong to any triangle" );
    }

    // Always have the same number of vertex data and vertices
    CORE_ASSERT( mesh.normals().size() == mesh.normals().size(), "Inconsistent number of normals" );
#endif
}

} // namespace MeshUtils
} // namespace Core
} // namespace Ra
