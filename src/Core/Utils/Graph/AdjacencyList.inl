#include <Core/Utils/Graph/AdjacencyList.hpp>
#include <iostream>
namespace Ra {
namespace Core {
namespace Graph {

    /// NODE
    inline uint AdjacencyList::addNode( const int parent ) {
        CORE_ASSERT( ( parent < size() ), "Index parent out of bounds" );
        uint idx = size();
        m_child.push_back( ChildrenList() ); // New node has no children
        // If is not a root node
        if( parent >= 0 ) {
            m_child[parent].push_back( idx );
        }
        m_parent.push_back( parent ); // Set new node parent
        return idx;
    }

    /// SIZE
    inline uint AdjacencyList::size() const {
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

    inline bool AdjacencyList::isEdge( const uint i, const uint j ) const {
        CORE_ASSERT( i < size(), " Index i out of bounds " );
        CORE_ASSERT( j < size(), " Index j out of bounds " );
        for( auto item : m_child[i] ) {
            if( item == j ) return true;
        }
        return false;
    }

} // namespace GraphicsEntity
} // namespace Core
} // namespace Ra

