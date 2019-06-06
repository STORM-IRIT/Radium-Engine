#ifndef POSE_H
#define POSE_H

#include <Core/Containers/AlignedStdVector.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * Definition of the Pose of a generic handle for model animation or deformation.
 */

/*
 * The Pose is represented just as a vector of Transforms.
 */
using Pose = AlignedStdVector<Eigen::Transform<Scalar, 3, Eigen::Affine>>;

// Typedef for code redability purposes only.
using RestPose = Pose;
using RefPose  = RestPose;

using LocalPose = Pose;
using ModelPose = Pose;

using RelativePose = Pose;

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // POSE_H
