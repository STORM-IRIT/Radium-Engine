#ifndef RADIUMENGINE_SKINNING_DATA_HPP_
#define RADIUMENGINE_SKINNING_DATA_HPP_

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Animation/Skeleton.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Skinning {

/// Skinning data that gets set at startup including the "reference state"
struct RefData {
    /// Skeleton
    Ra::Core::Animation::Skeleton m_skeleton;

    /// Mesh in reference position
    Ra::Core::Geometry::TriangleMesh m_referenceMesh;

    /// Reference pose
    Ra::Core::Animation::Pose m_refPose;

    /// Skinning weights.
    Ra::Core::Animation::WeightMatrix m_weights;

    /// The per-bone bind matrices.
    std::map<uint, Ra::Core::Transform> m_bindMatrices;

    /// Optionnal centers of rotations for CoR skinning
    Ra::Core::Vector3Array m_CoR;
};

/// Pose data of one frame. Poses are in model space
struct FrameData {
    /// Pose of the previous frame.
    Ra::Core::Animation::Pose m_previousPose;

    /// Pose of the current frame.
    Ra::Core::Animation::Pose m_currentPose;

    /// Relative pose from previous to current
    Ra::Core::Animation::Pose m_prevToCurrentRelPose;

    /// Relative pose from reference pose to current.
    Ra::Core::Animation::Pose m_refToCurrentRelPose;

    /// Previous position of the vertices
    Ra::Core::Vector3Array m_previousPos;

    /// Current position of the vertices
    Ra::Core::Vector3Array m_currentPos;

    /// Current vertex normals
    Ra::Core::Vector3Array m_currentNormal;

    /// Number of animation frames
    uint m_frameCounter;

    /// Indicator whether skinning must be processed.
    /// It is set to true if the current pose is different from previous.
    bool m_doSkinning;

    /// Indicator whether the skin must be reset to its initial reference
    /// configuration.
    bool m_doReset;
};

} // namespace Skinning
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_SKINNING_DATA_HPP_
