#include <Core/Utils/Graph/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Graph {

    /// SIZE
    uint AdjacencyList::size() const {
        return m_parent.size();
    }

    void AdjacencyList::clear() {
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

} // namespace GraphicsEntity
} // namespace Core
} // namespace Ra

