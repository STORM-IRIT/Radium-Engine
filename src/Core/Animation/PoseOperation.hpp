#ifndef POSE_OPERATION_H
#define POSE_OPERATION_H

#include <Core/Animation/Pose.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/// \name Pose Operations
/// \{

/**
 * Return true if the two poses are compatibile with each other.
 * A pose is compatible with another if the size of the two poses are equal.
 *
 * Possible FUTURE WORKS:
 * Two poses are compatible if their size is equal and if their transforms
 * are within the constraints of each others.
 */
RA_CORE_API bool compatible( const Pose& p0, const Pose& p1 );

/**
 * Given a model pose and a compatible rest pose, return the relative pose.
 */
RA_CORE_API Pose relativePose( const Pose& modelPose, const RestPose& restPose );

/**
 * Return the pose resulting in applying the given Transforms to the corresponging
 * Pose Tranforms.
 */
RA_CORE_API Pose applyTransformation( const Pose& pose,
                                      const AlignedStdVector<Transform>& transform );

/**
 * Return the pose resulting in applying the given transform to all the Pose Transforms.
 */
RA_CORE_API Pose applyTransformation( const Pose& pose, const Transform& transform );

/**
 * Return true if \p p0 and \p p1 are approximately equal, false otherwise.
 */
RA_CORE_API bool areEqual( const Pose& p0, const Pose& p1 );

/**
 * Return the pose interpolating \p a and \p b, w.r.t.\ the interpolation parameter \p t.
 */
RA_CORE_API Pose interpolatePoses( const Pose& a, const Pose& b, const Scalar t );

/**
 * Return in \p interpolated the Transform interpolating \p a and \p b,
 * w.r.t.\ the interpolation parameter \p t.
 */
RA_CORE_API void interpolateTransforms( const Ra::Core::Transform& a, const Ra::Core::Transform& b,
                                        Scalar t, Ra::Core::Transform& interpolated );
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // POSE_OPERATION_H
