#ifndef HANDLE_WEIGHT_H
#define HANDLE_WEIGHT_H

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Types.hpp>
#include <map>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * Defines the association animation handle / skinning weight for a vertex.
 */
using SingleWeight = std::pair<uint, Scalar>;

/**
 * Defines the list of all the skinning weights for a vertex.
 */
using VertexWeight = AlignedStdVector<SingleWeight>;

/**
 * Define the list of the skinning weights for the entire mesh.
 */
using MeshWeight = AlignedStdVector<VertexWeight>;

/**
 * Defines the weight matrix as an n x m matrix, where n is the number of
 * vertices and m the number of handles.
 * If w is a generic weight ( a.k.a. a scalar value ), then the WeightMatrix M
 * is defined as:
 *      M( i, j ) = w   , if vertex i is influenced by transform j ;
 *      M( i, j ) = 0   , otherwise
 */
using WeightMatrix = Ra::Core::Sparse;

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // HANDLE_WEIGHT_H
