#ifndef RADIUMENGINE_STBS_HPP
#define RADIUMENGINE_STBS_HPP

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Animation/Skeleton.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/DualQuaternion.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

using DQList = AlignedStdVector<DualQuaternion>;

/** \name STBS
 * Formula from: "Stretchable, Twistable Bones For Skeletal Shape Deformation" 2011 - Jacobson &
 * Sorkine.
 */
/// \{

void RA_CORE_API computeSTBS_weights( const Vector3Array& inMesh,
                                      const Ra::Core::Animation::Skeleton& skel,
                                      Ra::Core::Animation::WeightMatrix& weights );

/**
 * Apply the linear interpolation of transformations to the vertices of inMesh (STBS version).
 */
void RA_CORE_API linearBlendSkinningSTBS( const Vector3Array& inMesh,
                                          const Pose& pose,
                                          const Skeleton& poseSkel,
                                          const Skeleton& restSkel,
                                          const WeightMatrix& weightLBS,
                                          const WeightMatrix& weightSTBS,
                                          Vector3Array& outMesh );

/**
 * Computes the dual quaternions from the given pose, skeleton states and sets of skinning weights
 * (STBS version).
 */
void RA_CORE_API computeDQSTBS( const Pose& pose,
                                const Skeleton& poseSkel,
                                const Skeleton& restSkel,
                                const WeightMatrix& weight,
                                const WeightMatrix& weightSTBS,
                                DQList& DQ );
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_STBS_HPP
