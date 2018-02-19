#include "MeshUtils.hpp"
#include <Core/Geometry/PointCloud/PointCloud.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>
#include <Core/Utils/StdUtils.hpp>
#include <unordered_set>

namespace Ra {
namespace Core {
namespace MeshUtils {
inline Scalar getTriangleArea( const TriangleMesh& mesh, TriangleIdx triIdx ) {
    std::array<Vector3, 3> v;
    getTriangleVertices( mesh, triIdx, v );

    return Geometry::triangleArea( v[0], v[1], v[2] );
}

inline Vector3 getTriangleNormal( const TriangleMesh& mesh, TriangleIdx triIdx ) {
    std::array<Vector3, 3> v;
    getTriangleVertices( mesh, triIdx, v );
    return Geometry::triangleNormal( v[0], v[1], v[2] );
}

inline void getTriangleVertices( const TriangleMesh& mesh, TriangleIdx triIdx,
                                 std::array<Vector3, 3>& verticesOut ) {
    const Triangle& tri = mesh.m_triangles[triIdx];
    for ( uint i = 0; i < 3; ++i )
    {
        verticesOut[i] = mesh.vertices()[tri[i]];
    }
}

inline Aabb getAabb( const TriangleMesh& mesh ) {
    return PointCloud::aabb( mesh.vertices() );
}

inline uint getLastVertex( const Triangle& t1, uint v1, uint v2 ) {
    CORE_ASSERT( t1[0] == v1 || t1[1] == v1 || t1[2] == v1, "Vertex 1 not in triangle" );
    CORE_ASSERT( t1[0] == v2 || t1[1] == v2 || t1[2] == v2, "Vertex 2 not in triangle" );
    CORE_ASSERT( v1 != v2, "Vertices should be different " );
    if ( t1[0] == v1 )
    {
        return t1[1] == v2 ? t1[2] : t1[1];
    } else if ( t1[1] == v1 )
    {
        return t1[2] == v2 ? t1[0] : t1[2];
    } else // t1[2] ==v1
    { return t1[0] == v2 ? t1[1] : t1[0]; }
}

inline bool containsEdge( const Triangle& t1, uint v1, uint v2 ) {
    const bool hasv1 = ( t1.array() == Ra::Core::Vector3ui{v1, v1, v1}.array() ).any();
    const bool hasv2 = ( t1.array() == Ra::Core::Vector3ui{v2, v2, v2}.array() ).any();
    return hasv1 && hasv2;
}

struct EdgeEqual {
    bool operator()( const std::pair<uint, uint>& e1, const std::pair<uint, uint>& e2 ) const {
        return ( e1.first == e2.first && e1.second == e2.second ) ||
               ( e1.first == e2.second && e1.second == e2.first );
    }
};
struct EdgeHash {
    std::size_t operator()( const std::pair<uint, uint>& x ) const {
        return StdUtils::hash<uint, uint>( x );
    }
};

using EdgeTable = std::unordered_set<std::pair<uint, uint>, EdgeHash, EdgeEqual>;

std::vector<Ra::Core::Vector2ui> getEdges( const TriangleMesh& mesh ) {
    EdgeTable table;
    for ( const auto& t : mesh.m_triangles )
    {
        for ( uint i = 0; i < 3; ++i )
        {
            table.insert( {t[i], t[( i + 1 ) % 3]} );
        }
    }
    std::vector<Ra::Core::Vector2ui> edges;
    for ( const auto& e : table )
    {
        edges.push_back( {e.first, e.second} );
    }
    return edges;
}
} // namespace MeshUtils
} // namespace Core
} // namespace Ra
