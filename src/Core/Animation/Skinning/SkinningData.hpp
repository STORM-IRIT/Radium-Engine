#ifndef RADIUMENGINE_SKINNING_DATA_HPP_
#define RADIUMENGINE_SKINNING_DATA_HPP_

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra
{
namespace Core
{
namespace Skinning
{

    /// Skinning data that gets set at startup including the "reference state"
    struct RefData
    {
        /// Skeleton
        Ra::Core::Animation::Skeleton m_skeleton;

        /// Mesh in reference position
        Ra::Core::TriangleMesh m_referenceMesh;

        /// Reference pose
        Ra::Core::Animation::Pose m_refPose;

        /// Skinning weights.
        Ra::Core::Animation::WeightMatrix m_weights;
    };

    /// Pose data of one frame
    struct FrameData
    {
        /// Pose of the previous frame.
        Ra::Core::Animation::Pose m_previousPose;

        /// Pose of the current frame.
        Ra::Core::Animation::Pose m_currentPose;

        /// Relative pose from previous to current
        Ra::Core::Animation::Pose m_prevToCurrentRelPose;

        /// Relative pose from reference pose to current.
        Ra::Core::Animation::Pose m_refToCurrentRelPose;

        bool m_doSkinning;
    };

} // Skinning
} // Core
} // Ra


#endif //RADIUMENGINE_SKINNING_DATA_HPP_
