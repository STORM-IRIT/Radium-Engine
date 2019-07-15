#include "TriangleMesh.hpp"
#include <Core/Geometry/RayCast.hpp>
#include <Core/Geometry/TriangleOperation.hpp> // triangleArea
#include <Core/Types.hpp>

#include <array>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {

bool TriangleMesh::append( const TriangleMesh& other ) {
    // check same attributes through names
    if ( !m_vertexAttribs.hasSameAttribs( other.m_vertexAttribs ) ) return false;

    // now we can proceed, topology first
    const std::size_t verticesBefore  = vertices().size();
    const std::size_t trianglesBefore = m_triangles.size();
    const std::size_t facesBefore     = m_faces.size();
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
        if ( attr->isFloat() ) this->append_attrib<float>( attr );
        if ( attr->isVec2() ) this->append_attrib<Vector2>( attr );
        if ( attr->isVec3() ) this->append_attrib<Vector3>( attr );
        if ( attr->isVec4() ) this->append_attrib<Vector4>( attr );
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
            CORE_ASSERT( uint( tri[i] ) < nbVertices,
                         "Vertex " << tri[i] << " is in triangle " << t << " (#" << i
                                   << ") is out of bounds" );
            visited[tri[i]] = true;
        }
        CORE_WARN_IF( !( Geometry::triangleArea(
                             vertices()[tri[0]], vertices()[tri[1]], vertices()[tri[2]] ) > 0.f ),
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

void TriangleMesh::colorize( const Utils::Color& color ) {
    static const std::string colorAttribName( "in_color" );
    auto colorAttribHandle                = addAttrib<Core::Vector4>( colorAttribName );
    getAttrib( colorAttribHandle ).data() = Vector4Array( vertices().size(), color );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
