#ifndef POSE_H
#define POSE_H

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
* Definition of the Pose of a generic handle for model animation or deformation.
*/

/*
* The Pose is represented just as a vector of Transforms.
*/
typedef AlignedStdVector< Transform > Pose;

// Typedef for code redability purposes only.
typedef Pose     RestPose;
typedef RestPose RefPose;

typedef Pose LocalPose;
typedef Pose ModelPose;

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // POSE_H
