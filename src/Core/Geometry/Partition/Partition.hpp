#ifndef PARTITION_DEFINITION
#define PARTITION_DEFINITION

#include <set>

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

// Defining a Index ( useless ), representing the Index of a handle transform
typedef uint Index;

// Defining a set of indices, representing the indices of a set of handle transforms
typedef std::vector< Index > IndexSet;

// Defining the EdgeSet, representing a set of edges of the handle.
typedef VectorArray< Edge > EdgeSet;

// Defining the VertexSegment, representing a set of vertices indices of the mesh, hence a segment of the mesh
typedef std::set< VertexIdx > VertexSegment;

// Defining the TriangleSegment, representing a set of triangles indices of the mesh, hence a segment of the mesh
typedef std::set< TriangleIdx > TriangleSegment;

// Defining a BitSet, where true indicates that the i-th vertex is part of the segment
typedef std::vector< bool > BitSet;

}
}
}

#endif // PARTITION_DEFINITION
