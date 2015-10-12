#include <Core/Utils/Graph/AdjacencyList.hpp>
#include <iostream>
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
	
	inline int AdjacencyList::add(int parent)
	{
		CORE_ASSERT( parent < (int) size(), " Index parent out of bounds");
		int idx = (int) size();
		if (parent >= 0)
			m_child[parent].push_back(idx);
		m_parent.push_back(parent);
		m_child.push_back(ChildrenList());
		return idx;
	}

} // namespace GraphicsEntity
} // namespace Core
} // namespace Ra

