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
    m_vertexAttribs.clear();
    m_triangles = other.m_triangles;
    m_faces = other.m_faces;
    m_vertexAttribs.copyAllAttributes( other.m_vertexAttribs );
    m_verticesHandle = other.m_verticesHandle;
    m_normalsHandle = other.m_normalsHandle;
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
    if ( !m_vertexAttribs.hasSameAttribs( other.m_vertexAttribs ) )
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

inline void TriangleMesh::clear() {
    m_vertexAttribs.clear();
    m_triangles.clear();
    m_faces.clear();
    // restore the default attribs (empty though)
    initDefaultAttribs();
}

inline void TriangleMesh::clearAttributes() {
    Utils::Attrib<Vector3>::Container v;
    Utils::Attrib<Vector3>::Container n;
    std::exchange( v, vertices() );
    std::exchange( n, normals() );
    m_vertexAttribs.clear();
    initDefaultAttribs();
    std::exchange( vertices(), v );
    std::exchange( normals(), n );
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
