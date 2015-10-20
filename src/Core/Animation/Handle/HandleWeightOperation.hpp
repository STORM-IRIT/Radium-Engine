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



} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // HANDLE_WEIGHT_OPERATION

