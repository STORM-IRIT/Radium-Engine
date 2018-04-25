#ifndef ADJACENCY_LIST_OPERATION_H
#define ADJACENCY_LIST_OPERATION_H

#include <Core/Container/VectorArray.hpp>
#include <Core/Geometry/MeshTypes.hpp>

#include <Core/Utils/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/*
 * Check the integrity of the given AdjacencyList.
 */
RA_CORE_API void check( const AdjacencyList& adj );

/*
 * Return the adjacency list built from the given edge list.
 */
RA_CORE_API AdjacencyList extractAdjacencyList( const Container::VectorArray<Geometry::Edge>& edgeList );

/*
 * Return the edge list built from the given adjacency list.
 * If include_leaf is true, the list will contain the pairs:
 *       ...
 *       { i, i }
 *       ...
 * where i is the index of a leaf node.
 */
RA_CORE_API Container::VectorArray<Geometry::Edge> extractEdgeList( const AdjacencyList& adj,
                                               const bool include_leaf = false );

/*
 * Store the given AdjacencyList into a text file with the given name
 */
RA_CORE_API void storeAdjacencyList( const AdjacencyList& adj, const std::string& name );

} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // ADJACENCY_LIST_OPERATION_H
