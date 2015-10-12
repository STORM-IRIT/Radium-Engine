#ifndef PARTITION_OPERATION
#define PARTITION_OPERATION

#include <Core/Animation/Handle/HandleWeight.hpp>
#include "Partition.hpp"

namespace Ra {
namespace Core {
namespace Geometry {



/*
* Return the VertexSegment from the given set of weight, for the given id.
* If is_max is true, only the vertices where id is the most influent one will be returned.
*/
VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const Index& id, const bool is_max = true );



/*
* Return the VertexSegment from the given set of weight, for the given set of indices.
* If is_max is true, only the vertices where the indices are the most influent ones will be returned.
*/
VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const IndexSet& index, const bool is_max = true );



/*
* Return the VertexSegment from the given set of weight, for the given edge.
* If is_max is true, only the vertices where the edge is the most influent one will be returned.
*/
VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const Edge& edge, const bool is_max = true );



/*
* Return the VertexSegment from the given set of weight, for the given set of edges.
* If is_max is true, only the vertices where edges are the most influent ones will be returned.
*/
VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const EdgeSet& edge, const bool is_max = true );



/*
* Return the VertexSegment from the given set of weight, for the given id.
* If is_max is true, only the vertices where id is the most influent one will be returned.
*/
VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const Index& id, const bool is_max = true );



/*
* Return the VertexSegment from the given set of weight, for the given set of indices.
* If is_max is true, only the vertices where the indices are the most influent ones will be returned.
*/
VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const IndexSet& index, const bool is_max = true );



/*
* Return the VertexSegment from the given set of weight, for the given edge.
* If is_max is true, only the vertices where the edge is the most influent one will be returned.
*/
VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const Edge& edge, const bool is_max = true );



/*
* Return the VertexSegment from the given set of weight, for the given set of edges.
* If is_max is true, only the vertices where edges are the most influent ones will be returned.
*/
VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const EdgeSet& edge, const bool is_max = true );



/*
* Return the BitSet from the given VertexSegment, for a mesh with vertex_size vertices.
*/
BitSet extractBitSet( const VertexSegment& v, const uint vertex_size );



/*
* Return the TriangleSegment from the given BitSet, for the given triangles.
*/
TriangleSegment extractTriangleSegment( const BitSet& bit, const VectorArray< Triangle >& t );



}
}
}

#endif // PARTITION_OPERATION
