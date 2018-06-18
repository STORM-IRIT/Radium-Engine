#ifndef RADIUMENGINE_ROTATION_CENTER_SKINNING_HPP_
#define RADIUMENGINE_ROTATION_CENTER_SKINNING_HPP_

#include <Core/RaCore.hpp>

#include <Core/Animation/Skinning/SkinningData.hpp>

namespace Ra {
namespace Core {
namespace Animation {

// Optimal center of rotation skinning, based on Le & Hodgins
// "Real-time Skeletal Skinning with Optimized Centers of Rotation"
// ACM ToG, 2016.

/// Computes the similarity between two weights vector.
Scalar RA_CORE_API weightSimilarity( const Eigen::SparseVector<Scalar>& v1w,
                         const Eigen::SparseVector<Scalar>& v2w, Scalar sigma = 0.1f );

/// Compute the optimal center of rotations (1 per vertex) based on weight similarity.
void RA_CORE_API computeCoR( Skinning::RefData& dataInOut, Scalar sigma = 0.1f,
                             Scalar weightEpsilon = 0.1f );

/// Skin the vertices with the optimal centers of rotation.
void RA_CORE_API corSkinning( const Vector3Array& input, const Animation::Pose& pose,
                              const Animation::WeightMatrix& weight, const Vector3Array& CoR,
                              Vector3Array& output );

} // namespace Animation
} // namespace Core
} // namespace Ra
#endif // RADIUMENGINE_ROTATION_CENTER_SKINNING_HPP_
