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
    const std::size_t verticesBefore  = vertices().size();
    const std::size_t trianglesBefore = m_indices.size();

    // check same attributes through names
    if ( !AttribArrayGeometry::append( other ) ) return false;

    // now we can proceed topology
    m_indices.insert( m_indices.end(), other.m_indices.cbegin(), other.m_indices.cend() );

    // Offset the vertex indices in the triangles and faces
    for ( size_t t = trianglesBefore; t < m_indices.size(); ++t )
    {
        for ( uint i = 0; i < 3; ++i )
        {
            m_indices[t][i] += verticesBefore;
        }
    }

    return true;
}

/****/

bool LineMesh::append( const LineMesh& other ) {
    const std::size_t verticesBefore = vertices().size();
    const std::size_t linesBefore    = m_indices.size();

    // check same attributes through names
    if ( !AttribArrayGeometry::append( other ) ) return false;

    // now we can proceed topology
    m_indices.insert( m_indices.end(), other.m_indices.cbegin(), other.m_indices.cend() );

    // Offset the vertex indices in the lines and faces
    for ( size_t t = linesBefore; t < m_indices.size(); ++t )
    {
        for ( uint i = 0; i < 2; ++i )
        {
            m_indices[t][i] += verticesBefore;
        }
    }

    return true;
}

/****/

bool AttribArrayGeometry::append( const AttribArrayGeometry& other ) {
    // check same attributes through names
    if ( !m_vertexAttribs.hasSameAttribs( other.m_vertexAttribs ) ) return false;
    // Deal with all attributes the same way (vertices and normals too)
    other.m_vertexAttribs.for_each_attrib( [this]( const auto& attr ) {
        if ( attr->isFloat() ) this->append_attrib<float>( attr );
        if ( attr->isVec2() ) this->append_attrib<Vector2>( attr );
        if ( attr->isVec3() ) this->append_attrib<Vector3>( attr );
        if ( attr->isVec4() ) this->append_attrib<Vector4>( attr );
    } );

    return true;
}

void AttribArrayGeometry::clearAttributes() {
    PointAttribHandle::Container v  = vertices();
    NormalAttribHandle::Container n = normals();
    m_vertexAttribs.clear();
    initDefaultAttribs();
    setVertices( v );
    setNormals( n );
}

void TriangleMesh::checkConsistency() const {
#ifdef CORE_DEBUG
    const auto nbVertices = vertices().size();
    std::vector<bool> visited( nbVertices, false );
    for ( uint t = 0; t < m_indices.size(); ++t )
    {
        const Vector3ui& tri = m_indices[t];
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
    CORE_ASSERT( vertices().size() == normals().size(), "Inconsistent number of normals" );
#endif
}

void AttribArrayGeometry::colorize( const Utils::Color& color ) {
    static const std::string colorAttribName( "in_color" );
    auto colorAttribHandle = addAttrib<Core::Vector4>( colorAttribName );
    getAttrib( colorAttribHandle ).setData( Vector4Array( vertices().size(), color ) );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
