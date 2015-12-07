#ifndef HANDLE_WEIGHT_OPERATION
#define HANDLE_WEIGHT_OPERATION

#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/*
* Return the WeightMatrix extracted from the MeshWeight vector, for a handle with handle_size transforms
*/
WeightMatrix RA_CORE_API extractWeightMatrix( const MeshWeight& weight, const uint handle_size );



/*
* Return the MeshWeight from the given WeightMatrix.
*/
MeshWeight RA_CORE_API extractMeshWeight( const WeightMatrix& matrix );


/*
* Return the index of the weight that influence the most the position of vertex at vertexId.
*/
uint RA_CORE_API getMaxWeightIndex( const WeightMatrix& weights, const uint vertexID );



/*
* Return the vector containing the index of the handle influencing the most a vertex.
*/
void RA_CORE_API getMaxWeightIndex( const WeightMatrix& weights, std::vector< uint >& handleID );



void RA_CORE_API checkWeightMatrix( const WeightMatrix& matrix );

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // HANDLE_WEIGHT_OPERATION

