#ifndef ANIMPLUGIN_SKELETON_UTILS_HPP_
#define ANIMPLUGIN_SKELETON_UTILS_HPP_

//#include <Plugins/Animation/Pose/Pose.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

namespace Ra {
namespace Core {
namespace Animation
{
    namespace SkeletonUtils
    {
        /// Returns the start and end point of a bone in model space.
        inline void getBonePoints(const Ra::Core::Animation::Skeleton& skeleton, const Ra::Core::Animation::Pose* pose, int boneIdx,
                                    Ra::Core::Vector3& startOut, Ra::Core::Vector3& endOut)
        {
            // Check bone index is valid
            CORE_ASSERT(boneIdx >= 0 && boneIdx < skeleton.m_graph.size(), "invalid bone index");

            startOut = skeleton.getTransform(boneIdx, Ra::Core::Animation::Handle::SpaceType::MODEL).translation();
            auto children = skeleton.m_graph.m_child.at(boneIdx);

            // A leaf bone has length 0
            if (children.size() == 0)
            {
                endOut = startOut;
            }
            else
            {
                // End point is the average of chidren start points.
                endOut = Ra::Core::Vector3::Zero();
                for (auto child : children)
                {
                    endOut += skeleton.getTransform(child, Ra::Core::Animation::Handle::SpaceType::MODEL).translation();
                }

                endOut *= (1.f / children.size());
            }
        }

        /// Gives out the nearest point on a given bone.
        inline Ra::Core::Vector3 projectOnBone(const Ra::Core::Animation::Skeleton& skeleton, const Ra::Core::Animation::Pose* pose, int boneIdx, const Ra::Core::Vector3& pos)
        {
            Ra::Core::Vector3 start, end;
            getBonePoints(skeleton, pose, boneIdx, start, end);

            auto op = pos - start;
            auto dir = (end - start);
            // Square length of bone
            const Scalar length_sq = dir.squaredNorm();
            CORE_ASSERT(length_sq != 0.f, "bone has lenght 0, cannot project.");

            // Project on the line segment
            const Scalar t = Ra::Core::Math::clamp(op.dot(dir) / length_sq, 0.f, 1.f);
            return start + (t * dir);
        }
    }
}
}
}

#endif
