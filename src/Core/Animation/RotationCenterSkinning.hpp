#ifndef RADIUMENGINE_ROTATION_CENTER_SKINNING_HPP_
#define RADIUMENGINE_ROTATION_CENTER_SKINNING_HPP_

#include <Core/RaCore.hpp>

#include <array>

#include <Core/Container/IndexMap.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Geometry/DCEL/Dcel.hpp>
#include <Core/Geometry/DCEL/FullEdge.hpp>
#include <Core/Geometry/DCEL/Operations/EdgeSplit.hpp>
#include <Core/Geometry/MeshUtils.hpp>
#include <Core/Geometry/Convert.hpp>

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Animation/DualQuaternionSkinning.hpp>
#include <Core/Animation/LinearBlendSkinning.hpp>
#include <Core/Animation/SkinningData.hpp>

namespace Ra {
namespace Core {
namespace Animation {

// Optimal center of rotation skinning, based on Le & Hodgins
// "Real-time Skeletal Skinning with Optimized Centers of Rotation"
// ACM ToG, 2016.

/// Computes the similarity between two weights vector.
Scalar weightSimilarity( const Eigen::SparseVector<Scalar>& v1w,
                         const Eigen::SparseVector<Scalar>& v2w, Scalar sigma = 0.1f );

/// Compute the optimal center of rotations (1 per vertex) based on weight similarity.
void RA_CORE_API computeCoR( RefData& dataInOut, Scalar sigma = 0.1f,
                             Scalar weightEpsilon = 0.1f );

/// Skin the vertices with the optimal centers of rotation.
void RA_CORE_API corSkinning( const Container::Vector3Array& input, const Pose& pose,
                              const WeightMatrix& weight, const Container::Vector3Array& CoR,
                              Container::Vector3Array& output );

} // namespace Animation
} // namespace Core
} // namespace Ra
#endif // RADIUMENGINE_ROTATION_CENTER_SKINNING_HPP_
