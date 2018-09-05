#include "TriangleMesh.hpp"

namespace Ra {
namespace Core {

inline TriangleMesh::TriangleMesh( const TriangleMesh& other ) :
    m_triangles( other.m_triangles ),
    m_faces( other.m_faces ) {
    copyAttributes( other, other.m_verticesHandle, other.m_normalsHandle );
    m_verticesHandle =
        m_vertexAttribs.getAttribHandle<PointAttribHandle::value_type>( "in_position" );
    m_normalsHandle =
        m_vertexAttribs.getAttribHandle<NormalAttribHandle::value_type>( "in_normal" );
}

inline TriangleMesh::TriangleMesh( TriangleMesh&& other ) :
    m_triangles( std::move( other.m_triangles ) ),
    m_faces( std::move( other.m_faces ) ),
    m_vertexAttribs( std::move( other.m_vertexAttribs ) ),
    m_verticesHandle( std::move( other.m_verticesHandle ) ),
    m_normalsHandle( std::move( other.m_normalsHandle ) ) {}

inline TriangleMesh& TriangleMesh::operator=( const TriangleMesh& other ) {
    m_triangles = other.m_triangles;
    m_faces = other.m_faces;
    copyAttributes( other, other.m_verticesHandle, other.m_normalsHandle );
    m_verticesHandle =
        m_vertexAttribs.getAttribHandle<PointAttribHandle::value_type>( "in_position" );
    m_normalsHandle =
        m_vertexAttribs.getAttribHandle<NormalAttribHandle::value_type>( "in_normal" );
}

inline TriangleMesh& TriangleMesh::operator=( TriangleMesh&& other ) {
    m_triangles = std::move( other.m_triangles );
    m_faces = std::move( other.m_faces );
    m_vertexAttribs = std::move( other.m_vertexAttribs );
    m_verticesHandle = std::move( other.m_verticesHandle );
    m_normalsHandle = std::move( other.m_normalsHandle );
}

inline void TriangleMesh::clear() {
    m_vertexAttribs.clear();
    vertices().clear();
    normals().clear();
    m_triangles.clear();

    initDefaultAttribs();
}

inline void TriangleMesh::append( const TriangleMesh& other ) {
    const std::size_t verticesBefore = vertices().size();
    const std::size_t trianglesBefore = m_triangles.size();

    vertices().insert( vertices().end(), other.vertices().cbegin(), other.vertices().cend() );
    ///\todo what about other attribs ?"
    normals().insert( normals().end(), other.normals().cbegin(), other.normals().cend() );
    m_triangles.insert( m_triangles.end(), other.m_triangles.cbegin(), other.m_triangles.cend() );

    // Offset the vertex indices in the faces
    for ( uint t = trianglesBefore; t < m_triangles.size(); ++t )
    {
        for ( uint i = 0; i < 3; ++i )
        {
            m_triangles[t][i] += verticesBefore;
        }
    }
}

template <typename... Handles>
void TriangleMesh::copyAttributes( const TriangleMesh& input, Handles... attribs ) {
    // copy attribs
    m_vertexAttribs.copyAttributes( input.m_vertexAttribs, attribs... );
    // update custom handles
    m_verticesHandle =
        m_vertexAttribs.getAttribHandle<PointAttribHandle::value_type>( "in_position" );
    m_normalsHandle =
        m_vertexAttribs.getAttribHandle<NormalAttribHandle::value_type>( "in_normal" );
}

inline void TriangleMesh::copyAllAttributes( const TriangleMesh& input ) {
    // copy attribs
    m_vertexAttribs.copyAllAttributes( input.m_vertexAttribs );
    // update custom handles
    m_verticesHandle =
        m_vertexAttribs.getAttribHandle<PointAttribHandle::value_type>( "in_position" );
    m_normalsHandle =
        m_vertexAttribs.getAttribHandle<NormalAttribHandle::value_type>( "in_normal" );
}

} // namespace Core
} // namespace Ra
