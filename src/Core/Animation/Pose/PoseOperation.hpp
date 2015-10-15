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
bool compatible( const Pose& p0, const Pose& p1 );



/*
* Given a model pose and a compatible rest pose, return the relative pose.
*/
Pose relativePose( const Pose& modelPose, const RestPose& restPose );



/*
* Return the pose resulting in applying the i-th transform to the i-th pose transform.
*
* The operation is equal to:
*           pose[i] = transform[i] * pose[i];
*/
Pose applyTransformation( const Pose& pose, const std::vector< Transform >& transform );



/*
* Return the pose resulting in applying the transform to the pose transforms.
*
* The operation is equal to:
*           pose[i] = transform * pose[i];
*/
Pose applyTransformation( const Pose& pose, const Transform& transform );

Pose interpolatePoses(const Pose& a, const Pose& b, Scalar t);

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // POSE_OPERATION_H
