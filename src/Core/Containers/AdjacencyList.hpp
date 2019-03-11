#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Eigen/Core>
#include <iostream>

namespace Ra {
namespace Core {

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
    /**
     * The possible consistency statuses.
     */
    enum class ConsistencyStatus {
        Valid,
        IncompatibleChildrenAndParentList,
        WrongParentOrdering,
        WrongParentIndex,
        InconsistentParentIndex,
        NonLeafNodeWithoutChild
    };

    AdjacencyList();

    AdjacencyList( const uint n );

    AdjacencyList( const AdjacencyList& adj );

    ~AdjacencyList();

    /// \name Nodes
    /// \{

    /**
     * Return the index of the added leaf. Use -1 to create the root node.
     */
    uint addNode( const int parent );

    /**
     * Prune the leaves of the graph and returns the changes.
     */
    void pruneLeaves( std::vector<uint>& pruned, std::vector<bool>& delete_flag );

    /**
     * Prune the leaves of the graph.
     */
    void pruneLeaves();

    /**
     * Clear all data.
     */
    inline void clear();
    /// \}

    /// \name Adjacency
    /// \{

    /**
     * Return the edge list built from the given adjacency list.
     * If include_leaf is true, the list will contain the pairs:
     *       ...
     *       { i, i }
     *       ...
     * where i is the index of a leaf node.
     */
    VectorArray<Eigen::Matrix<uint, 2, 1>> extractEdgeList( bool include_leaf = false ) const;

    /**
     * Returns the per-element lists of children.
     */
    inline const Adjacency& children() const;

    /**
     * Returns the list of per-element parent.
     */
    inline const ParentList& parents() const;
    /// \}

    /// \name Status queries
    /// \{

    /**
     * Return the number of nodes in the graph.
     */
    inline uint size() const;

    /**
     * Return true if the graph is consistent
     */
    ConsistencyStatus computeConsistencyStatus() const;

    /**
     * Return true if the graph is empty.
     */
    inline bool isEmpty() const;

    /**
     * Return true if a node is a root node.
     */
    inline bool isRoot( const uint i ) const;

    /**
     * Return true if the node is a leaf node.
     */
    inline bool isLeaf( const uint i ) const;

    /**
     * Return true if the node is a branch node ( |child| > 1 ).
     */
    inline bool isBranch( const uint i ) const;

    /**
     * Return true if the node is a joint node ( |child| == 1 ).
     */
    inline bool isJoint( const uint i ) const;

    /**
     * Return true if the edge { i, j } exists.
     */
    inline bool isEdge( const uint i, const uint j ) const;
    /// \}

  private:
    /// Adjacency matrix
    Adjacency m_child;

    /// Parents ids vector
    ParentList m_parent;

    /// Depth of the nodes.
    LevelList m_level;
};

/// Stream insertion operator.
RA_CORE_API std::ofstream& operator<<( std::ofstream& ofs, const AdjacencyList& p );

} // namespace Core
} // namespace Ra

#include <Core/Containers/AdjacencyList.inl>

#endif // ADJACENCY_LIST_H
