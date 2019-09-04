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
    m_normalsHandle  = other.m_normalsHandle;
}

inline TriangleMesh::TriangleMesh( TriangleMesh&& other ) :
    AbstractGeometry( other ),
    m_triangles( std::move( other.m_triangles ) ),
    m_faces( std::move( other.m_faces ) ),
    m_vertexAttribs( std::move( other.m_vertexAttribs ) ),
    m_verticesHandle( std::move( other.m_verticesHandle ) ),
    m_normalsHandle( std::move( other.m_normalsHandle ) ) {}

inline TriangleMesh& TriangleMesh::operator=( const TriangleMesh& other ) {
    if ( this != &other )
    {
        m_vertexAttribs.clear();
        m_triangles = other.m_triangles;
        m_faces     = other.m_faces;
        m_vertexAttribs.copyAllAttributes( other.m_vertexAttribs );
        m_verticesHandle = other.m_verticesHandle;
        m_normalsHandle  = other.m_normalsHandle;
    }
    return *this;
}

inline TriangleMesh& TriangleMesh::operator=( TriangleMesh&& other ) {
    if ( this != &other )
    {
        m_triangles      = std::move( other.m_triangles );
        m_faces          = std::move( other.m_faces );
        m_vertexAttribs  = std::move( other.m_vertexAttribs );
        m_verticesHandle = std::move( other.m_verticesHandle );
        m_normalsHandle  = std::move( other.m_normalsHandle );
    }
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
    m_faces     = other.m_faces;
    m_vertexAttribs.copyAttributes(
        other.m_vertexAttribs, other.m_verticesHandle, other.m_normalsHandle );
}

template <typename... Handles>
bool TriangleMesh::copyAttributes( const TriangleMesh& input, Handles... attribs ) {
    if ( vertices().size() != input.vertices().size() ) return false;
    // copy attribs
    m_vertexAttribs.copyAttributes( input.m_vertexAttribs, attribs... );
    return true;
}

inline bool TriangleMesh::copyAllAttributes( const TriangleMesh& input ) {
    if ( vertices().size() != input.vertices().size() ) return false;
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

inline void TriangleMesh::setVertices( PointAttribHandle::Container&& vertices ) {
    m_vertexAttribs.setAttrib( m_verticesHandle, std::move( vertices ) );
}

inline void TriangleMesh::setVertices( const PointAttribHandle::Container& vertices ) {
    m_vertexAttribs.setAttrib<PointAttribHandle::value_type>( m_verticesHandle, vertices );
}

inline const TriangleMesh::PointAttribHandle::Container& TriangleMesh::vertices() const {
    return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
}

inline void TriangleMesh::setNormals( PointAttribHandle::Container&& normals ) {
    m_vertexAttribs.setAttrib( m_normalsHandle, std::move( normals ) );
}
inline void TriangleMesh::setNormals( const PointAttribHandle::Container& normals ) {
    m_vertexAttribs.setAttrib( m_normalsHandle, normals );
}

inline const TriangleMesh::NormalAttribHandle::Container& TriangleMesh::normals() const {
    return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
}

template <typename T>
Utils::AttribHandle<T> TriangleMesh::getAttribHandle( const std::string& name ) const {
    return m_vertexAttribs.findAttrib<T>( name );
}

template <typename T>
bool TriangleMesh::isValid( const Utils::AttribHandle<T>& h ) const {
    return m_vertexAttribs.isValid( h );
}

template <typename T>
Utils::Attrib<T>& TriangleMesh::getAttrib( const Utils::AttribHandle<T>& h ) {
    return m_vertexAttribs.getAttrib( h );
}

template <typename T>
const Utils::Attrib<T>& TriangleMesh::getAttrib( const Utils::AttribHandle<T>& h ) const {
    return m_vertexAttribs.getAttrib( h );
}

Utils::AttribBase* TriangleMesh::getAttribBase( const std::string& name ) {
    return m_vertexAttribs.getAttribBase( name );
}

bool TriangleMesh::hasAttrib( const std::string& name ) {
    return m_vertexAttribs.contains( name );
}

template <typename T>
Utils::AttribHandle<T> TriangleMesh::addAttrib( const std::string& name ) {
    return m_vertexAttribs.addAttrib<T>( name );
}

template <typename T>
void TriangleMesh::removeAttrib( Utils::AttribHandle<T>& h ) {
    m_vertexAttribs.removeAttrib( h );
}

inline Utils::AttribManager& TriangleMesh::vertexAttribs() {
    return m_vertexAttribs;
}

inline TriangleMesh::PointAttribHandle::Container& TriangleMesh::verticesWithLock() {
    return m_vertexAttribs.getAttrib( m_verticesHandle ).getDataWithLock();
}

inline void TriangleMesh::verticesUnlock() {
    return m_vertexAttribs.getAttrib( m_verticesHandle ).unlock();
}

inline TriangleMesh::NormalAttribHandle::Container& TriangleMesh::normalsWithLock() {
    return m_vertexAttribs.getAttrib( m_normalsHandle ).getDataWithLock();
}

inline void TriangleMesh::normalsUnlock() {
    return m_vertexAttribs.getAttrib( m_normalsHandle ).unlock();
}

inline void TriangleMesh::initDefaultAttribs() {
    m_verticesHandle = m_vertexAttribs.addAttrib<PointAttribHandle::value_type>( "in_position" );
    m_normalsHandle  = m_vertexAttribs.addAttrib<NormalAttribHandle::value_type>( "in_normal" );
}

template <typename T>
void TriangleMesh::append_attrib( Utils::AttribBase* attr ) {
    auto h         = m_vertexAttribs.findAttrib<T>( attr->getName() );
    auto& v0       = m_vertexAttribs.getAttrib( h ).getDataWithLock();
    const auto& v1 = attr->cast<T>().data();
    v0.insert( v0.end(), v1.cbegin(), v1.cend() );
    m_vertexAttribs.getAttrib( h ).unlock();
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
