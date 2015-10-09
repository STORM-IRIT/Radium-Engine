#ifndef ADJACENCY_LIST_OPERATION_H
#define ADJACENCY_LIST_OPERATION_H

#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshTypes.hpp>

#include <Core/Utils/Graph/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Graph {


/*
* Check the integrity of the given AdjacencyList.
*/
void check( const AdjacencyList& adj );



/*
* Return the adjacency list built from the given edge list.
*/
AdjacencyList extractAdjacencyList( const VectorArray< Edge >& edgeList );



/*
* Return the edge list built from the given adjacency list.
*/
VectorArray< Edge > extractEdgeList( const AdjacencyList& adj );



} // namespace GraphicsEntity
} // namespace Core
} // namespace Ra

#endif // ADJACENCY_LIST_OPERATION_H
