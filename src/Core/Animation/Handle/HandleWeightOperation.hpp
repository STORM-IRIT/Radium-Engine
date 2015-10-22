#ifndef HANDLE_WEIGHT_OPERATION
#define HANDLE_WEIGHT_OPERATION

#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/*
* Return the WeightMatrix extracted from the MeshWeight vector, for a handle with handle_size transforms
*/
WeightMatrix extractWeightMatrix( const MeshWeight& weight, const uint handle_size );



/*
* Return the MeshWeight from the given WeightMatrix.
*/
MeshWeight extractMeshWeight( const WeightMatrix& matrix );

int getMaxWeightIndex(const WeightMatrix& weights, int vertexId);

void getMaxWeightIndex(const WeightMatrix& weights, std::vector<uint>& boneIds);

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // HANDLE_WEIGHT_OPERATION

