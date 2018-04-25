#include <Core/Utils/AdjacencyList.hpp>

#include <set>

namespace Ra {
namespace Core {
namespace Utils {

/// NODE
inline uint AdjacencyList::addNode( const int parent ) {
    CORE_ASSERT( ( parent < int( size() ) ), "Index parent out of bounds" );
    uint idx = size();
    m_child.push_back( ChildrenList() ); // New node has no children
    // If is not a root node
    if ( parent >= 0 )
    {
        m_child[parent].push_back( idx );
        m_level.push_back( m_level[parent] + 1 );
    } else
    { m_level.push_back( 0 ); }
    m_parent.push_back( parent ); // Set new node parent
    return idx;
}

inline void AdjacencyList::pruneLeaves( std::vector<uint>& pruned,
                                        std::vector<bool>& delete_flag ) {
    pruned.clear();
    delete_flag.clear();
    delete_flag.resize( this->size(), false );
    std::vector<bool> prune_flag = delete_flag;
    for ( uint i = 0; i < this->size(); ++i )
    {
        if ( this->isLeaf( i ) && !this->isRoot( i ) )
        {
            delete_flag[i] = true;
            prune_flag[i] = true;
        } else
        { pruned.push_back( i ); }
    }

    for ( uint j = this->size(); j > 0; --j )
    {
        const uint i = j - 1;
        if ( prune_flag[i] )
        {
            this->m_parent.erase( this->m_parent.begin() + i );
            this->m_child.erase( this->m_child.begin() + i );
            prune_flag.erase( prune_flag.begin() + i );
            ++j;
        }
    }

    for ( uint i = 0; i < this->size(); ++i )
    {
        this->m_parent[i] =
            ( ( this->m_parent[i] == -1 ) || ( delete_flag[i] ) ) ? -1 : pruned[this->m_parent[i]];
        for ( auto it = this->m_child[i].begin(); it != this->m_child[i].end(); ++it )
        {
            if ( delete_flag[( *it )] )
            {
                this->m_child[i].erase( it );
                --it;
            } else
            { *it = pruned[*it]; }
        }
    }
}

inline void AdjacencyList::pruneLeaves() {
    std::vector<uint> p;
    std::vector<bool> d;
    this->pruneLeaves( p, d );
}

/// SIZE
inline uint AdjacencyList::size() const {
    CORE_ASSERT( m_parent.size() == m_child.size(), "List size inconsistency" );
    return m_parent.size();
}

inline void AdjacencyList::clear() {
    m_child.clear();
    m_parent.clear();
}

/// QUERY
inline bool AdjacencyList::isEmpty() const {
    return ( size() == 0 );
}

inline bool AdjacencyList::isRoot( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_parent.at( i ) == -1 );
}

inline bool AdjacencyList::isLeaf( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_child.at( i ).size() == 0 );
}

inline bool AdjacencyList::isBranch( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_child.at( i ).size() > 1 );
}

inline bool AdjacencyList::isJoint( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_child.at( i ).size() == 1 );
}

inline bool AdjacencyList::isEdge( const uint i, const uint j ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    CORE_ASSERT( j < size(), " Index j out of bounds " );
    for ( const auto& item : m_child[i] )
    {
        if ( item == j )
            return true;
    }
    return false;
}

} // namespace Utils
} // namespace Core
} // namespace Ra
