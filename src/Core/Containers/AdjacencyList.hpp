#pragma once

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Eigen/Core>
#include <iostream>

namespace Ra {
namespace Core {

using ParentList   = AlignedStdVector<int>;
using LevelList    = AlignedStdVector<uint8_t>;
using ChildrenList = AlignedStdVector<uint8_t>;
using Adjacency    = AlignedStdVector<ChildrenList>;

/**
 * The AdjacencyList contains the adjacency matrix expressed as a vector of indices and
 * a vector containing the index of the parents indices of the i-th node.
 */
class RA_CORE_API AdjacencyList
{
  public:
    enum class ConsistencyStatus {
        Valid,
        IncompatibleChildrenAndParentList,
        WrongParentOrdering,
        WrongParentIndex,
        InconsistentParentIndex,
        NonLeafNodeWithoutChild
    };

    //////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTOR
    //////////////////////////////////////////////////////////////////////////////
    AdjacencyList();
    explicit AdjacencyList( const uint n );
    AdjacencyList( const AdjacencyList& adj );
    AdjacencyList& operator=( const AdjacencyList& ) = default;

    //////////////////////////////////////////////////////////////////////////////
    // DESTRUCTOR
    //////////////////////////////////////////////////////////////////////////////
    ~AdjacencyList();

    //////////////////////////////////////////////////////////////////////////////
    // NODE
    //////////////////////////////////////////////////////////////////////////////
    /// Return the index of the added root.
    uint addRoot();
    /// Return the index of the added leaf.
    uint addNode( const uint parent );
    /// Prune the leaves of the graph and returns the changes.
    void pruneLeaves( std::vector<uint>& pruned, std::vector<bool>& delete_flag );
    /// Prune the leaves of the graph.
    void pruneLeaves();
    /*!
     * Return the edge list built from the given adjacency list.
     * If include_leaf is true, the list will contain the pairs:
     *       ...
     *       { i, i }
     *       ...
     * where i is the index of a leaf node.
     */
    VectorArray<Eigen::Matrix<uint, 2, 1>> extractEdgeList( bool include_leaf = false ) const;

    //////////////////////////////////////////////////////////////////////////////
    // SIZE
    //////////////////////////////////////////////////////////////////////////////
    /// Return the number of nodes in the graph
    inline uint size() const;
    /// Clear the vectors
    inline void clear();

    //////////////////////////////////////////////////////////////////////////////
    // QUERY
    //////////////////////////////////////////////////////////////////////////////
    /// Return true if the graph is consistent
    ConsistencyStatus computeConsistencyStatus() const;
    /// Return true if the graph is empty.
    inline bool isEmpty() const { return ( size() == 0 ); }
    /// Return true if a node is a root node.
    inline bool isRoot( const uint i ) const;
    /// Return true if the node is a leaf node.
    inline bool isLeaf( const uint i ) const;
    /// Return true if the node is a branch node. ( |child| > 1 )
    inline bool isBranch( const uint i ) const;
    /// Return true if the node is a joint node. ( |child| == 1 )
    inline bool isJoint( const uint i ) const;
    /// Return true if the edge { i, j } exists.
    inline bool isEdge( const uint i, const uint j ) const;

    inline const Adjacency& children() const { return m_child; }

    inline const ParentList& parents() const { return m_parent; }

    //////////////////////////////////////////////////////////////////////////////
    // VARIABLE
    //////////////////////////////////////////////////////////////////////////////
  private:
    /// Adjacency matrix
    Adjacency m_child;
    /// Parents ids vector
    ParentList m_parent;
    LevelList m_level;
};

RA_CORE_API std::ofstream& operator<<( std::ofstream& ofs, const AdjacencyList& p );

inline uint AdjacencyList::size() const {
    CORE_ASSERT( m_parent.size() == m_child.size(), "List size inconsistency" );
    return m_parent.size();
}

inline void AdjacencyList::clear() {
    m_child.clear();
    m_parent.clear();
}

inline bool AdjacencyList::isRoot( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_parent[i] == -1 );
}

inline bool AdjacencyList::isLeaf( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_child[i].size() == 0 );
}

inline bool AdjacencyList::isBranch( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_child[i].size() > 1 );
}

inline bool AdjacencyList::isJoint( const uint i ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    return ( m_child[i].size() == 1 );
}

inline bool AdjacencyList::isEdge( const uint i, const uint j ) const {
    CORE_ASSERT( i < size(), " Index i out of bounds " );
    CORE_ASSERT( j < size(), " Index j out of bounds " );
    for ( const auto& item : m_child[i] ) {
        if ( item == j ) return true;
    }
    return false;
}
} // namespace Core
} // namespace Ra
