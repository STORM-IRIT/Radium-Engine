#include "TriangleMesh.hpp"

namespace Ra {
namespace Core {

inline TriangleMesh::TriangleMesh( const TriangleMesh& other ) :
    m_triangles( other.m_triangles ),
    m_faces( other.m_faces ) {
    m_vertexAttribs.copyAllAttributes( other.m_vertexAttribs );
    m_verticesHandle = other.m_verticesHandle;
    m_normalsHandle = other.m_normalsHandle;
}

inline TriangleMesh::TriangleMesh( TriangleMesh&& other ) :
    m_triangles( std::move( other.m_triangles ) ),
    m_faces( std::move( other.m_faces ) ),
    m_vertexAttribs( std::move( other.m_vertexAttribs ) ),
    m_verticesHandle( std::move( other.m_verticesHandle ) ),
    m_normalsHandle( std::move( other.m_normalsHandle ) ) {}

inline TriangleMesh& TriangleMesh::operator=( const TriangleMesh& other ) {
    clear();
    m_triangles = other.m_triangles;
    m_faces = other.m_faces;
    m_vertexAttribs.copyAllAttributes( other.m_vertexAttribs );
    return *this;
}

inline TriangleMesh& TriangleMesh::operator=( TriangleMesh&& other ) {
    m_triangles = std::move( other.m_triangles );
    m_faces = std::move( other.m_faces );
    m_vertexAttribs = std::move( other.m_vertexAttribs );
    m_verticesHandle = std::move( other.m_verticesHandle );
    m_normalsHandle = std::move( other.m_normalsHandle );
    return *this;
}

inline bool TriangleMesh::append( const TriangleMesh& other ) {
    // check same attributes through names
    bool sameAttrib = true;
    const auto& m_attr = m_vertexAttribs.m_attribsIndex;
    const auto& o_attr = other.m_vertexAttribs.m_attribsIndex;
    // one way
    for ( const auto& attr : m_attr )
    {
        if ( o_attr.find( attr.first ) == o_attr.end() )
        {
            sameAttrib = false;
            break;
        }
    }
    if ( !sameAttrib )
        return false;
    // the other way
    for ( const auto& attr : o_attr )
    {
        if ( m_attr.find( attr.first ) == m_attr.end() )
        {
            sameAttrib = false;
            break;
        }
    }
    if ( !sameAttrib )
        return false;

    // now we can proceed, topology first
    const std::size_t verticesBefore = vertices().size();
    const std::size_t trianglesBefore = m_triangles.size();
    const std::size_t facesBefore = m_faces.size();
    m_triangles.insert( m_triangles.end(), other.m_triangles.cbegin(), other.m_triangles.cend() );
    m_faces.insert( m_faces.end(), other.m_faces.cbegin(), other.m_faces.cend() );
    // Offset the vertex indices in the triangles and faces
    for ( uint t = trianglesBefore; t < m_triangles.size(); ++t )
    {
        for ( uint i = 0; i < 3; ++i )
        {
            m_triangles[t][i] += verticesBefore;
        }
    }
    for ( uint f = facesBefore; f < m_faces.size(); ++f )
    {
        for ( uint i = 0; i < m_faces[f].size(); ++i )
        {
            m_faces[f][i] += verticesBefore;
        }
    }

    // Deal with all attributes the same way (vertices and normals too)
    for ( auto& attr : other.m_vertexAttribs.attribs() )
    {
        if ( attr->isFloat() )
        {
            auto h = m_vertexAttribs.findAttrib<float>( attr->getName() );
            auto& v0 = static_cast<Attrib<float>*>( &( m_vertexAttribs.getAttrib( h ) ) )->data();
            const auto& v1 = static_cast<Attrib<float>*>( attr )->data();
            v0.insert( v0.end(), v1.cbegin(), v1.cend() );
        } else if ( attr->isVec2() )
        {
            auto h = m_vertexAttribs.findAttrib<Vector2>( attr->getName() );
            auto& v0 = static_cast<Attrib<Vector2>*>( &( m_vertexAttribs.getAttrib( h ) ) )->data();
            const auto& v1 = static_cast<Attrib<Vector2>*>( attr )->data();
            v0.insert( v0.end(), v1.cbegin(), v1.cend() );
        } else if ( attr->isVec3() )
        {
            auto h = m_vertexAttribs.findAttrib<Vector3>( attr->getName() );
            auto& v0 = static_cast<Attrib<Vector3>*>( &( m_vertexAttribs.getAttrib( h ) ) )->data();
            const auto& v1 = static_cast<Attrib<Vector3>*>( attr )->data();
            v0.insert( v0.end(), v1.cbegin(), v1.cend() );
        } else if ( attr->isVec4() )
        {
            auto h = m_vertexAttribs.findAttrib<Vector4>( attr->getName() );
            auto& v0 = static_cast<Attrib<Vector4>*>( &( m_vertexAttribs.getAttrib( h ) ) )->data();
            const auto& v1 = static_cast<Attrib<Vector4>*>( attr )->data();
            v0.insert( v0.end(), v1.cbegin(), v1.cend() );
        }
    }

    return true;
}

inline void TriangleMesh::clear() {
    m_vertexAttribs.clear();
    m_triangles.clear();
    m_faces.clear();
    // restore the default attribs (empty though)
    initDefaultAttribs();
}

inline void TriangleMesh::clearAttributes() {
    const auto v = vertices();
    const auto n = normals();
    m_vertexAttribs.clear();
    initDefaultAttribs();
    vertices() = v;
    normals() = n;
}

inline void TriangleMesh::copyBaseGeometry( const TriangleMesh& other ) {
    clear();
    m_triangles = other.m_triangles;
    m_faces = other.m_faces;
    m_vertexAttribs.copyAttributes( other.m_vertexAttribs, other.m_verticesHandle,
                                    other.m_normalsHandle );
}

template <typename... Handles>
bool TriangleMesh::copyAttributes( const TriangleMesh& input, Handles... attribs ) {
    if ( vertices().size() != input.vertices().size() )
        return false;
    // copy attribs
    m_vertexAttribs.copyAttributes( input.m_vertexAttribs, attribs... );
    return true;
}

inline bool TriangleMesh::copyAllAttributes( const TriangleMesh& input ) {
    if ( vertices().size() != input.vertices().size() )
        return false;
    // copy attribs
    m_vertexAttribs.copyAllAttributes( input.m_vertexAttribs );
    return true;
}

} // namespace Core
} // namespace Ra
