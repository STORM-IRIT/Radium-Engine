#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H

#include <Core/Containers/AlignedStdVector.hpp>

namespace Ra {
namespace Core {
namespace Graph {

using ParentList = AlignedStdVector<int>;
using LevelList = AlignedStdVector<uint8_t>;
using ChildrenList = AlignedStdVector<uint8_t>;
using Adjacency = AlignedStdVector<ChildrenList>;

/**
 * The AdjacencyList contains the adjacency matrix expressed as a vector of indices and
 * a vector containing the index of the parents indices of the i-th node.
 */
class RA_CORE_API AdjacencyList {
  public:
    AdjacencyList();

    AdjacencyList( const uint n );

    AdjacencyList( const AdjacencyList& adj );

    ~AdjacencyList();

    /// Return the index of the added leaf. Use -1 to create the root node.
    inline uint addNode( const int parent );

    /// Prune the leaves of the graph and returns the changes.
    inline void pruneLeaves( std::vector<uint>& pruned, std::vector<bool>& delete_flag );

    /// Prune the leaves of the graph.
    inline void pruneLeaves();

    /// Return the number of nodes in the graph.
    inline uint size() const;

    /// Clear the vectors
    inline void clear();

    /// Return true if the graph is empty.
    inline bool isEmpty() const;

    /// Return true if a node is a root node.
    inline bool isRoot( const uint i ) const;

    /// Return true if the node is a leaf node.
    inline bool isLeaf( const uint i ) const;

    /// Return true if the node is a branch node (i.e. |child| > 1 ).
    inline bool isBranch( const uint i ) const;

    /// Return true if the node is a joint node (i.e. |child| == 1 ).
    inline bool isJoint( const uint i ) const;

    /// Return true if the edge { i, j } exists.
    inline bool isEdge( const uint i, const uint j ) const;

    /// Adjacency matrix.
    Adjacency m_child;

    /// Parents ids.
    ParentList m_parent;

    /// Depth of the nodes.
    LevelList m_level;
};

} // namespace Graph
} // namespace Core
} // namespace Ra

#include <Core/Utils/Graph/AdjacencyList.inl>

#endif // ADJACENCY_LIST_H
