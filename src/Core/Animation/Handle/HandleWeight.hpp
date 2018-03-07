#ifndef HANDLE_WEIGHT_H
#define HANDLE_WEIGHT_H

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <utility>

namespace Ra {
namespace Core {
namespace Animation {

// Taken from the ISL. TODO: Keep only one version
using SkinningWeights = std::map<int, float>;
using Weights = std::vector<SkinningWeights>;

// Defining the pair handle/weight for a vertex
using SingleWeight = std::pair<uint, Scalar>;

// Defining the vector of the skinning weights for a vertex
using VertexWeight = AlignedStdVector<SingleWeight>;

// Defining the vector of the skinning weights for the entire mesh
using MeshWeight = AlignedStdVector<VertexWeight>;

// Defining the weight matrix as the nxm matrix, where n are the vertices and m the handles
// It is like an adjacency matrix between vertices and handle transforms.
// If w is a generic weight ( a.k.a. a scalar value ), then the WeightMatrix M is defined as:
//      M( i, j ) = w   , if vertex i is influenced by transform j
//      M( i, j ) = 0   , otherwise
using WeightMatrix = Sparse;

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // HANDLE_WEIGHT_H
