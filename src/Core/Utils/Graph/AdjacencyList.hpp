#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H

#include <Core/Containers/AlignedStdVector.hpp>

namespace Ra {
namespace Core {
namespace Graph {

typedef AlignedStdVector< int >          ParentList;
typedef AlignedStdVector< uint >         LevelList;
typedef AlignedStdVector< uint >         ChildrenList;
typedef AlignedStdVector< ChildrenList > Adjacency;

/**
* The AdjacencyList contains the adjacency matrix expressed as a vector of indices and
* a vector containing the index of the parents indices of the i-th node.
*/
class /*RA_CORE_API*/ AdjacencyList {
public:
    //////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTOR
    //////////////////////////////////////////////////////////////////////////////
    AdjacencyList();
    AdjacencyList( const uint n );
    AdjacencyList( const AdjacencyList& adj );

    //////////////////////////////////////////////////////////////////////////////
    // DESTRUCTOR
    //////////////////////////////////////////////////////////////////////////////
    ~AdjacencyList();

    //////////////////////////////////////////////////////////////////////////////
    // NODE
    //////////////////////////////////////////////////////////////////////////////
    inline uint addNode( const int parent); // Return the index of the added leaf. Use -1 to create the root node.
    inline void pruneLeaves( std::vector<uint>& pruned, std::vector<bool>& delete_flag ); // Prune the leaves of the graph and returns the changes.
    inline void pruneLeaves(); // Prune the leaves of the graph.

    //////////////////////////////////////////////////////////////////////////////
    // SIZE
    //////////////////////////////////////////////////////////////////////////////
    inline uint size() const; // Return the number of nodes in the graph
    inline void clear();      // Clear the vectors

    //////////////////////////////////////////////////////////////////////////////
    // QUERY
    //////////////////////////////////////////////////////////////////////////////
    inline bool isEmpty() const;                            // Return true if the graph is empty.
    inline bool isRoot( const uint i ) const;               // Return true if a node is a root node.
    inline bool isLeaf( const uint i ) const;               // Return true if the node is a leaf node.
    inline bool isBranch( const uint i ) const;             // Return true if the node is a branch node. ( |child| > 1 )
    inline bool isJoint( const uint i ) const;              // Return true if the node is a joint node. ( |child| == 1 )
    inline bool isEdge( const uint i, const uint j ) const; // Return true if the edge { i, j } exists.


    //////////////////////////////////////////////////////////////////////////////
    // VARIABLE
    //////////////////////////////////////////////////////////////////////////////
    Adjacency    m_child;  // Adjacency matrix
    ParentList   m_parent; // Parents ids vector
    LevelList    m_level;
};

} // namespace Graph
} // namespace Core
} // namespace Ra

#include <Core/Utils/Graph/AdjacencyList.inl>

#endif // ADJACENCY_LIST_H
