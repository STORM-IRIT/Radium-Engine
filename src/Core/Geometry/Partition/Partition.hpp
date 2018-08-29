#ifndef PARTITION_DEFINITION
#define PARTITION_DEFINITION

#include <set>

#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

// Defining a Index ( useless ), representing the Index of a handle transform
using Index = uint;

// Defining the VertexSegment, representing a set of vertices indices of the mesh, hence a segment
// of the mesh
using VertexSegment = std::vector<VertexIdx>;

// Defining the TriangleSegment, representing a set of triangles indices of the mesh, hence a
// segment of the mesh
using TriangleSegment = std::vector<TriangleIdx>;

// Defining a BitSet, where true indicates that the i-th vertex is part of the segment
using BitSet = std::vector<bool>;

using MeshPartition = std::vector<TriangleMesh>;

/**
 * Return the VertexSegment from the given set of weight, for the given id.
 * If use_max is true, only the vertices where id is the most influent one will be returned.
 * Otherwise the lattest id met will be returned.
 */
VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const Index& id,
                                    const bool use_max = true );

/**
 * Return the VertexSegment from the given set of weight, for the given id.
 * If is_max is true, only the vertices where id is the most influent one will be returned.
 */
VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const Index& id,
                                    const bool use_max = true );

/**
 * Return the BitSet from the given VertexSegment, for a mesh with vertex_size vertices.
 */
BitSet extractBitSet( const VertexSegment& v, const uint vertex_size );

/**
 * Return the TriangleSegment from the given BitSet, for the given triangles.
 */
TriangleSegment extractTriangleSegment( const BitSet& bit, const VectorArray<Triangle>& t );

/**
 * Return the partitioning of \p mesh according to the weight in \p weight.
 * If use_max is true, partitioning will use the bone with the most influence.
 * Otherwise the lattest bone met will be use.
 */
MeshPartition partition( const TriangleMesh& mesh, const Animation::WeightMatrix& weight,
                         const bool use_max = true );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // PARTITION_DEFINITION
