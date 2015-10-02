#ifndef ANIMPLUGIN_SKELETON_UTILS_HPP_
#define ANIMPLUGIN_SKELETON_UTILS_HPP_

#include <Plugins/Animation/Pose/Pose.hpp>
namespace AnimationPlugin
{
    namespace Skeleton_utils
    {
        /// Returns the start and end point of a bone in model space.
        inline void getBonePoints(const Pose* pose, int boneIdx,
                                    Ra::Core::Vector3& startOut, Ra::Core::Vector3& endOut)
        {
            // Check bone index is valid
            CORE_ASSERT(boneIdx >= 0 && boneIdx < pose->get_skeleton()->getNumBones(), "invalid bone index");

            startOut = pose->get_pose_bone<Pose::MODEL>(boneIdx).get_translation();
            auto children = pose->get_skeleton()->getChildrenIdx(boneIdx);

            // A leaf bone has length 0
            if (children.size() == 0)
            {
                endOut = startOut;
            }
            else
            {
                // End point is the average of chidren start points.
                endOut = Ra::Core::Vector3::Zeros();
                for (auto child : children)
                {
                    endOut += pose->get_pose_bone<Pose::MODEL>(child).get_translation();
                }

                endOut *= (1.f / children.size());
            }
        }

        /// Gives out the nearest point on a given bone.
        inline Ra::Core::Vector3 projectOnBone(const Pose* pose, int boneIdx, const Ra::Core::Vector3& pos)
        {
            Ra::Core::Vector3 start, end;
            getBonePoints(pose, boneIdx, start, end);

            auto op = pos - start;
            auto dir = (end - start);
            // Square length of bone
            const Scalar length_sq = dir.squaredNorm();
            CORE_ASSERT(length_sq != 0.f, "bone has lenght 0, cannot project.");

            // Project on the line segment
            const float t = Ra::Core::Vector::clamp(op.dot(dir) / length_sq, 0.f, 1.f);
            return start + (t * dir);
        }
    }
}
#endif
