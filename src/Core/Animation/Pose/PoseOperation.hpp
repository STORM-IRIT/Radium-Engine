#ifndef POSE_OPERATION_H
#define POSE_OPERATION_H

#include <Core/Animation/Pose/Pose.hpp>

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

RA_CORE_API void interpolateTransforms( const Ra::Core::Transform& a, const Ra::Core::Transform& b,
                                        Scalar t, Ra::Core::Transform& interpolated );

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // POSE_OPERATION_H
