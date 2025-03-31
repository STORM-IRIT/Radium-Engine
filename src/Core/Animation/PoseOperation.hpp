#pragma once

#include <Core/Animation/Pose.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/CoreMacros.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/*
 * Return true if the two poses are compatibile with each other.
 * A pose is compatible with another if the size of the two poses are equal.
 *
 * Possible FUTURE WORKS:
 * Two poses are compatible if their size is equal and if their transforms
 * are within the constraints of each others.
 */
RA_CORE_API bool compatible( const Pose& p0, const Pose& p1 );

/*
 * Given a model pose and a compatible rest pose, return the relative pose.
 */
RA_CORE_API Pose relativePose( const Pose& modelPose, const RestPose& restPose );

/*
 * Return the pose resulting in applying the i-th transform to the i-th pose transform.
 *
 * The operation is equal to:
 *           pose[i] = transform[i] * pose[i];
 */
RA_CORE_API Pose applyTransformation( const Pose& pose,
                                      const AlignedStdVector<Transform>& transform );

/*
 * Return the pose resulting in applying the transform to the pose transforms.
 *
 * The operation is equal to:
 *           pose[i] = transform * pose[i];
 */
RA_CORE_API Pose applyTransformation( const Pose& pose, const Transform& transform );

RA_CORE_API bool areEqual( const Pose& p0, const Pose& p1 );

RA_CORE_API Pose interpolatePoses( const Pose& a, const Pose& b, const Scalar t );

} // namespace Animation
} // namespace Core
} // namespace Ra
