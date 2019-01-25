#include "TriangleMesh.hpp"

namespace Ra {
namespace Core {
namespace Geometry {

inline TriangleMesh::TriangleMesh( const TriangleMesh& other ) :
    AbstractGeometry(),
    m_triangles( other.m_triangles ),
    m_faces( other.m_faces ) {
    m_vertexAttribs.copyAllAttributes( other.m_vertexAttribs );
    m_verticesHandle = other.m_verticesHandle;
    m_normalsHandle = other.m_normalsHandle;
}

inline TriangleMesh::TriangleMesh( TriangleMesh&& other ) :
    AbstractGeometry( other ),
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

inline void TriangleMesh::clear() {
    m_vertexAttribs.clear();
    m_triangles.clear();
    m_faces.clear();
    // restore the default attribs (empty though)
    initDefaultAttribs();
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

inline Aabb TriangleMesh::computeAabb() const {
    Aabb aabb;
    for ( const auto& v : vertices() )
    {
        aabb.extend( v );
    }
    return aabb;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
