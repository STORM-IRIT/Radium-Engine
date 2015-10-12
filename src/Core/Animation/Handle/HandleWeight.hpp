#ifndef HANDLE_WEIGHT_H
#define HANDLE_WEIGHT_H

#include <utility>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Animation {

// Defining the pair handle/weight for a vertex
typedef std::pair< uint, Scalar > SingleWeight;

// Defining the vector of the skinning weights for a vertex
typedef AlignedStdVector< SingleWeight > VertexWeight;

// Defining the vector of the skinning weights for the entire mesh
typedef AlignedStdVector< VertexWeight > MeshWeight;

// Defining the weight matrix as the nxm matrix, where n are the vertices and m the handles
// It is like an adjacency matrix between vertices and handle transforms.
// If w is a generic weight ( a.k.a. a scalar value ), then the WeightMatrix M is defined as:
//      M( i, j ) = w   , if vertex i is influenced by transform j
//      M( i, j ) = 0.0 , otherwise
typedef Sparse WeightMatrix;



} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // HANDLE_WEIGHT_H

