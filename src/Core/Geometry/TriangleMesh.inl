#include "TriangleMesh.hpp"
#include <Core/Geometry/TriangleOperation.hpp> // triangleArea

namespace Ra {
namespace Core {
namespace Geometry {

inline AttribArrayGeometry ::AttribArrayGeometry( const AttribArrayGeometry& other ) :
    AbstractGeometry( other ) {
    m_vertexAttribs.copyAllAttributes( other.m_vertexAttribs );
    m_verticesHandle = other.m_verticesHandle;
    m_normalsHandle  = other.m_normalsHandle;
}

inline AttribArrayGeometry::AttribArrayGeometry( AttribArrayGeometry&& other ) :
    m_vertexAttribs( std::move( other.m_vertexAttribs ) ),
    m_verticesHandle( std::move( other.m_verticesHandle ) ),
    m_normalsHandle( std::move( other.m_normalsHandle ) ) {}

inline AttribArrayGeometry& AttribArrayGeometry::operator=( const AttribArrayGeometry& other ) {
    if ( this != &other )
    {
        m_vertexAttribs.clear();
        m_vertexAttribs.copyAllAttributes( other.m_vertexAttribs );
        m_verticesHandle = other.m_verticesHandle;
        m_normalsHandle  = other.m_normalsHandle;
    }
    return *this;
}

inline AttribArrayGeometry& AttribArrayGeometry::operator=( AttribArrayGeometry&& other ) {
    if ( this != &other )
    {
        m_vertexAttribs  = std::move( other.m_vertexAttribs );
        m_verticesHandle = std::move( other.m_verticesHandle );
        m_normalsHandle  = std::move( other.m_normalsHandle );
    }
    return *this;
}

inline void AttribArrayGeometry::clear() {
    m_vertexAttribs.clear();
    // restore the default attribs (empty though)
    initDefaultAttribs();
}

inline void AttribArrayGeometry::copyBaseGeometry( const AttribArrayGeometry& other ) {
    clear();
    m_vertexAttribs.copyAttributes(
        other.m_vertexAttribs, other.m_verticesHandle, other.m_normalsHandle );
}

template <typename... Handles>
inline bool AttribArrayGeometry::copyAttributes( const AttribArrayGeometry& input,
                                                 Handles... attribs ) {
    if ( vertices().size() != input.vertices().size() ) return false;
    // copy attribs
    m_vertexAttribs.copyAttributes( input.m_vertexAttribs, attribs... );
    return true;
}

inline bool AttribArrayGeometry::copyAllAttributes( const AttribArrayGeometry& input ) {
    if ( vertices().size() != input.vertices().size() ) return false;
    // copy attribs
    m_vertexAttribs.copyAllAttributes( input.m_vertexAttribs );
    return true;
}

inline Aabb AttribArrayGeometry::computeAabb() const {
    Aabb aabb;
    for ( const auto& v : vertices() )
    {
        aabb.extend( v );
    }
    return aabb;
}

inline void AttribArrayGeometry::setVertices( PointAttribHandle::Container&& vertices ) {
    m_vertexAttribs.setAttrib( m_verticesHandle, std::move( vertices ) );
}

inline void AttribArrayGeometry::setVertices( const PointAttribHandle::Container& vertices ) {
    m_vertexAttribs.setAttrib<PointAttribHandle::value_type>( m_verticesHandle, vertices );
}

inline const AttribArrayGeometry::PointAttribHandle::Container&
AttribArrayGeometry::vertices() const {
    return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
}

inline void AttribArrayGeometry::setNormals( PointAttribHandle::Container&& normals ) {
    m_vertexAttribs.setAttrib( m_normalsHandle, std::move( normals ) );
}
inline void AttribArrayGeometry::setNormals( const PointAttribHandle::Container& normals ) {
    m_vertexAttribs.setAttrib( m_normalsHandle, normals );
}

inline const AttribArrayGeometry::NormalAttribHandle::Container&
AttribArrayGeometry::normals() const {
    return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
}

template <typename T>
inline Utils::AttribHandle<T>
AttribArrayGeometry::getAttribHandle( const std::string& name ) const {
    return m_vertexAttribs.findAttrib<T>( name );
}

template <typename T>
inline bool AttribArrayGeometry::isValid( const Utils::AttribHandle<T>& h ) const {
    return m_vertexAttribs.isValid( h );
}

template <typename T>
inline Utils::Attrib<T>& AttribArrayGeometry::getAttrib( const Utils::AttribHandle<T>& h ) {
    return m_vertexAttribs.getAttrib( h );
}

template <typename T>
const Utils::Attrib<T>& AttribArrayGeometry::getAttrib( const Utils::AttribHandle<T>& h ) const {
    return m_vertexAttribs.getAttrib( h );
}

inline Utils::AttribBase* AttribArrayGeometry::getAttribBase( const std::string& name ) {
    return m_vertexAttribs.getAttribBase( name );
}

inline bool AttribArrayGeometry::hasAttrib( const std::string& name ) {
    return m_vertexAttribs.contains( name );
}

template <typename T>
inline Utils::AttribHandle<T> AttribArrayGeometry::addAttrib( const std::string& name ) {
    return m_vertexAttribs.addAttrib<T>( name );
}

template <typename T>
inline Utils::AttribHandle<T>
AttribArrayGeometry::addAttrib( const std::string& name,
                                const typename Core::VectorArray<T>& data ) {
    auto handle = addAttrib<T>( name );
    getAttrib( handle ).setData( data );
    return handle;
}

template <typename T>
inline Utils::AttribHandle<T>
AttribArrayGeometry::addAttrib( const std::string& name,
                                const typename Utils::Attrib<T>::Container&& data ) {
    auto handle = addAttrib<T>( name );
    getAttrib( handle ).setData( std::move( data ) );
    return handle;
}

template <typename T>
inline void AttribArrayGeometry::removeAttrib( Utils::AttribHandle<T>& h ) {
    m_vertexAttribs.removeAttrib( h );
}

inline Utils::AttribManager& AttribArrayGeometry::vertexAttribs() {
    return m_vertexAttribs;
}

inline const Utils::AttribManager& AttribArrayGeometry::vertexAttribs() const {
    return m_vertexAttribs;
}

inline AttribArrayGeometry::PointAttribHandle::Container& AttribArrayGeometry::verticesWithLock() {
    return m_vertexAttribs.getAttrib( m_verticesHandle ).getDataWithLock();
}

inline void AttribArrayGeometry::verticesUnlock() {
    return m_vertexAttribs.getAttrib( m_verticesHandle ).unlock();
}

inline AttribArrayGeometry::NormalAttribHandle::Container& AttribArrayGeometry::normalsWithLock() {
    return m_vertexAttribs.getAttrib( m_normalsHandle ).getDataWithLock();
}

inline void AttribArrayGeometry::normalsUnlock() {
    return m_vertexAttribs.getAttrib( m_normalsHandle ).unlock();
}

inline void AttribArrayGeometry::initDefaultAttribs() {
    m_verticesHandle = m_vertexAttribs.addAttrib<PointAttribHandle::value_type>( "in_position" );
    m_normalsHandle  = m_vertexAttribs.addAttrib<NormalAttribHandle::value_type>( "in_normal" );
}

template <typename T>
inline void AttribArrayGeometry::append_attrib( Utils::AttribBase* attr ) {
    auto h         = m_vertexAttribs.findAttrib<T>( attr->getName() );
    auto& v0       = m_vertexAttribs.getAttrib( h ).getDataWithLock();
    const auto& v1 = attr->cast<T>().data();
    v0.insert( v0.end(), v1.cbegin(), v1.cend() );
    m_vertexAttribs.getAttrib( h ).unlock();
}

/*** IndexedGeometry ***/
template <typename T>
inline IndexedGeometry<T>::IndexedGeometry( const IndexedGeometry<IndexType>& other ) :
    AttribArrayGeometry( other ), m_indices( other.m_indices ) {}

template <typename T>
inline IndexedGeometry<T>::IndexedGeometry( IndexedGeometry<IndexType>&& other ) :
    AttribArrayGeometry( std::move( other ) ), m_indices( std::move( other.m_indices ) ) {}

template <typename T>
inline IndexedGeometry<T>&
IndexedGeometry<T>::operator=( const IndexedGeometry<IndexType>& other ) {
    AttribArrayGeometry::operator=( other );
    m_indices                    = other.m_indices;
    return *this;
}

template <typename T>
inline IndexedGeometry<T>& IndexedGeometry<T>::operator=( IndexedGeometry<IndexType>&& other ) {
    AttribArrayGeometry::operator=( std::move( other ) );
    m_indices                    = std::move( other.m_indices );
    return *this;
}

template <typename T>
inline void IndexedGeometry<T>::clear() {
    m_indices.clear();
    AttribArrayGeometry::clear();
}

template <typename T>
inline void IndexedGeometry<T>::copy( const IndexedGeometry<IndexType>& other ) {
    AttribArrayGeometry::copyBaseGeometry( other );
    m_indices = other.m_indices;
}

template <typename T>
inline void IndexedGeometry<T>::checkConsistency() const {
#ifdef CORE_DEBUG
    const auto nbVertices = vertices().size();
    std::vector<bool> visited( nbVertices, false );
    for ( uint t = 0; t < m_indices.size(); ++t )
    {
        const IndexType& face = m_indices[t];
        for ( uint i = 0; i < IndexType::RowsAtCompileTime; ++i )
        {
            CORE_ASSERT( uint( face[i] ) < nbVertices,
                         "Vertex " << face[i] << " is out of bound, in face " << t << " (#" << i
                                   << ")" );
            visited[face[i]] = true;
        }
        CORE_WARN_IF( IndexType::RowsAtCompileTime == 3 &&
                          !( Geometry::triangleArea( vertices()[face[0]],
                                                     vertices()[face[1]],
                                                     vertices()[face[2]] ) > 0.f ),
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

template <typename T>
inline bool IndexedGeometry<T>::append( const IndexedGeometry<IndexType>& other ) {
    const std::size_t verticesBefore  = vertices().size();
    const std::size_t trianglesBefore = m_indices.size();

    // check same attributes through names
    if ( !AttribArrayGeometry::append( other ) ) return false;

    // now we can proceed topology
    m_indices.insert( m_indices.end(), other.m_indices.cbegin(), other.m_indices.cend() );

    // Offset the vertex indices in the triangles and faces
    for ( size_t t = trianglesBefore; t < m_indices.size(); ++t )
    {
        for ( uint i = 0; i < IndexType::RowsAtCompileTime; ++i )
        {
            m_indices[t][i] += verticesBefore;
        }
    }

    return true;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
