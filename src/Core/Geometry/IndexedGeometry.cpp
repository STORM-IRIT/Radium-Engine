#include <Core/Geometry/IndexedGeometry.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

MultiIndexedGeometry::MultiIndexedGeometry( const MultiIndexedGeometry& other ) :
    AttribArrayGeometry( other ), m_indices( other.m_indices ) {}

MultiIndexedGeometry::MultiIndexedGeometry( MultiIndexedGeometry&& other ) :
    AttribArrayGeometry( std::move( other ) ), m_indices( std::move( other.m_indices ) ) {}

MultiIndexedGeometry::MultiIndexedGeometry( const AttribArrayGeometry& other ) :
    AttribArrayGeometry( other ) {}

MultiIndexedGeometry::MultiIndexedGeometry( AttribArrayGeometry&& other ) :
    AttribArrayGeometry( std::move( other ) ) {}

MultiIndexedGeometry& MultiIndexedGeometry::operator=( const MultiIndexedGeometry& other ) {
    AttribArrayGeometry::operator=( other );
    m_indices                    = other.m_indices;
    notify();
    return *this;
}

MultiIndexedGeometry& MultiIndexedGeometry::operator=( MultiIndexedGeometry&& other ) {
    AttribArrayGeometry::operator=( std::move( other ) );
    m_indices                    = std::move( other.m_indices );
    notify();
    return *this;
}

void MultiIndexedGeometry::clear() {
    m_indices.clear();
    AttribArrayGeometry::clear();
    notify();
}

void MultiIndexedGeometry::copy( const MultiIndexedGeometry& other ) {
    AttribArrayGeometry::copyBaseGeometry( other );
    m_indices = other.m_indices;
    notify();
}

void MultiIndexedGeometry::checkConsistency() const {
#ifdef CORE_DEBUG
    // const auto nbVertices = vertices().size();
    // std::vector<bool> visited( nbVertices, false );
    // for ( uint t = 0; t < m_indices.size(); ++t )
    // {
    //     const IndexType& face = m_indices[t];
    //     for ( uint i = 0; i < IndexType::RowsAtCompileTime; ++i )
    //     {
    //         CORE_ASSERT( uint( face[i] ) < nbVertices,
    //                      "Vertex " << face[i] << " is out of bound, in face " << t << " (#" << i
    //                                << ")" );
    //         visited[face[i]] = true;
    //     }
    //     CORE_WARN_IF( IndexType::RowsAtCompileTime == 3 &&
    //                       !( Geometry::triangleArea( vertices()[face[0]],
    //                                                  vertices()[face[1]],
    //                                                  vertices()[face[2]] ) > 0.f ),
    //                   "triangle " << t << " is degenerate" );
    // }

    // for ( uint v = 0; v < nbVertices; ++v )
    // {
    //     CORE_ASSERT( visited[v], "Vertex " << v << " does not belong to any triangle" );
    // }

    // // Always have the same number of vertex data and vertices
    // CORE_ASSERT( vertices().size() == normals().size(), "Inconsistent number of normals" );
#endif
}

// todo: append only shared indices
// inline bool MultiIndexedGeometry::append( const MultiIndexedGeometry& other ) {
//     const std::size_t verticesBefore  = vertices().size();
//     const std::size_t trianglesBefore = m_indices.size();

//     // check same attributes through names
//     if ( !AttribArrayGeometry::append( other ) ) return false;

//     // now we can proceed topology
//     m_indices.insert( m_indices.end(), other.m_indices.cbegin(), other.m_indices.cend() );

//     // Offset the vertex indices in the triangles and faces
//     for ( size_t t = trianglesBefore; t < m_indices.size(); ++t )
//     {
//         for ( uint i = 0; i < IndexType::RowsAtCompileTime; ++i )
//         {
//             m_indices[t][i] += verticesBefore;
//         }
//     }
//     notify();
//     return true;
// }

bool MultiIndexedGeometry::indicesExists( const IndicesSemanticCollection& semantics ) const {
    return m_indices.find( semantics ) != m_indices.end();
}

const IndexViewBase&
MultiIndexedGeometry::getIndices( const IndicesSemanticCollection& semantics ) const {
    return m_indices.at( semantics ).second;
}

IndexViewBase&
MultiIndexedGeometry::getIndicesWithLock( const IndicesSemanticCollection& semantics ) {
    auto& p = m_indices.at( semantics );
    CORE_ASSERT( !p.first, "try to get already locked indices" );
    p.first = true;
    return p.second;
}

void MultiIndexedGeometry::indicesUnlock( const IndicesSemanticCollection& semantics ) {
    auto& p = m_indices.at( semantics );
    CORE_ASSERT( p.first, "try to unlock not locked indices" );
    p.first = false;
    notify();
}

void MultiIndexedGeometry::setIndices( const IndexViewBase& indices ) {
    const auto& key = indices.semantics();
    auto it         = m_indices.find( key );
    if ( it != m_indices.end() )
    {
        CORE_ASSERT( !( *it ).second.first, "try to set already locked indices" );
        ( *it ).second.second = std::move( indices );
    }
    else
        m_indices.insert( {key, std::make_pair( false, std::move( indices ) )} );
    notify();
}

} // namespace Geometry
} // namespace Core
} // namespace Ra