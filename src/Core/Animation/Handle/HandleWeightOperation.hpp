#ifndef HANDLE_WEIGHT_OPERATION
#define HANDLE_WEIGHT_OPERATION

#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/*
 * Return the WeightMatrix extracted from the MeshWeight vector, for a handle with handle_size
 * transforms
 */
WeightMatrix RA_CORE_API extractWeightMatrix( const MeshWeight& weight, const uint handle_size );

/*
 * Return the MeshWeight from the given WeightMatrix.
 */
MeshWeight RA_CORE_API extractMeshWeight( Eigen::Ref<const WeightMatrix> matrix );

/*
 * Return the WeightMatrix holding the partition of unity property.
 * This is obtained by normalizing each row by its l1-norm, assuming:
 *       weights( i, j ) >= 0
 *       lpNorm1( weights.row( i ) ) > 0
 */
WeightMatrix RA_CORE_API partitionOfUnity( Eigen::Ref<const WeightMatrix> weights );

/*
 * Return the index of the weight that influence the most the position of vertex at vertexId.
 */
uint RA_CORE_API getMaxWeightIndex( Eigen::Ref<const WeightMatrix> weights, const uint vertexID );

/*
 * Return the vector containing the index of the handle influencing the most a vertex.
 */
void RA_CORE_API getMaxWeightIndex( Eigen::Ref<const WeightMatrix> weights,
                                    std::vector<uint>& handleID );

/*
 * Call MatrixUtils::checkInvalidNumbers and checkNoWeightVertex on the input matrix
 */
bool RA_CORE_API checkWeightMatrix( Eigen::Ref<const WeightMatrix> matrix,
                                    const bool FAIL_ON_ASSERT = false, const bool MT = false );

bool RA_CORE_API checkNoWeightVertex( Eigen::Ref<const WeightMatrix> matrix,
                                      const bool FAIL_ON_ASSERT = false, const bool MT = false );

/**
 * In-place normalization of the weights, such that matrix.row( k ).sum() = 1;
 * \return true if normalization was required.
 */
bool RA_CORE_API normalizeWeights( Eigen::Ref<WeightMatrix> matrix, const bool MT = false );

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // HANDLE_WEIGHT_OPERATION
