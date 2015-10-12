#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H

#include <Core/Containers/AlignedStdVector.hpp>

namespace Ra {
namespace Core {
namespace Graph {

typedef AlignedStdVector< int >          ParentList;
typedef AlignedStdVector< uint >         ChildrenList;
typedef AlignedStdVector< ChildrenList > Adjacency;

/**
* The AdjacencyList contains the adjacency matrix expressed as a vector of indices and
* a vector containing the index of the parents indices of the i-th node.
*/
class AdjacencyList {
public:
    /// CONSTRUCTOR
    AdjacencyList();
    AdjacencyList( const uint n );
    AdjacencyList( const AdjacencyList& adj );

    /// DESTRUCTOR
    ~AdjacencyList();

    /// SIZE
    inline uint size() const; // Return the number of nodes in the graph
    inline void clear();      // Clear the vectors

    /// QUERY
    inline bool isEmpty() const;                // Return true if the graph is empty.
    inline bool isRoot( const uint i ) const;   // Return true if a node is a root node.
    inline bool isLeaf( const uint i ) const;   // Return true if the node is a leaf node.

	int add(int parent); // Return the index of the added leaf. Use -1 to create the root node.
	
    /// VARIABLE
    Adjacency  m_child;  // Adjacency matrix
    ParentList m_parent; // Parents ids vector
};

} // namespace GraphicsEntity
} // namespace Core
} // namespace Ra

#include <Core/Utils/Graph/AdjacencyList.inl>

#endif // ADJACENCY_LIST_H
