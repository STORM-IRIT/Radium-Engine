#ifndef POSED_SKELETON_HPP_
#define POSED_SKELETON_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Core/Containers/AlignedStdVector.hpp>

#include <Plugins/Animation/Skeleton/Skeleton.hpp>

namespace AnimationPlugin
{
/// This class stores both local and model space pose and keeps them in sync for
/// a given skeleton.
    class Pose
    {
    public:
        /// The two pose modes.
        enum Mode
        {
            LOCAL = 0,  // Each transform is relative to its parent
            MODEL       // Each transform is in the skeleton's model space
        };


    public:
        /// Constructs a posed skeleton with the given pose.
        Pose(Mode mode, const Skeleton* skel, const RawPose& pose);

        Pose(const Pose& pose) = default;

        Pose& operator=(const Pose& pose) = default;

        /// Change the whole pose according to the given mode and recompute the other.
        template<Mode MODE>
        inline void setPose(const RawPose& pose);

        //
        // Accessors.
        //

        /// Const accessors for the whole poses
        template<Mode MODE>
        inline const RawPose& getPose() const;

        /// Const accessors for one bone
        template<Mode MODE>
        inline const Ra::Core::Transform& getBoneTransform(int boneIdx) const;

        inline const Skeleton* getSkeleton() const;

        //
        // Animation and relative poses
        //


        /// Resets the pose to the skeleton's rest pose.
        inline void resetPose();

        /// Directly sets bone local transform (thus moving all children)
        inline void setBoneTransform(int boneIdx, const Ra::Core::Transform& tr);

        // These two need to be transformed in "rotate bone".
        /// Apply the transform to the current transform at the given bone.
        inline void applyRelativeBoneTransform(int boneIdx, const Ra::Core::Transform& tr);

        inline void applyAbsoluteBoneTransform(int boneIdx, const Ra::Core::Transform& tr);

        /// Returns the relative transforms between pose start and end.
        /// basically for each bone transform T_i, T_i_end = T_i_rel * T_i_start.
        template<Mode MODE>
        static void getRelativePose(RawPose& relPoseOut,
                                      const Pose* poseStart,
                                      const Pose* poseEnd);

        /// Ra::Core::Transformrms the current pose with the given relative pose.
        template<Mode MODE>
        void applyRelativePose(const RawPose& relPose);

        //
        // Maintaining consistency
        //

        /// Recomputes the local pose from the model
        void recomputeLocal();

        /// Recomputes the model pose from the local
        void recomputeModel();


        /// Do some consistency checks, and assert if wrong in debug mode
        void check() const;

    private:
        /// Pose in local space, relative to parent
        /// Pose in model space
        RawPose m_poses[2];
        const Skeleton* m_skel; /// Skeleton.
    };
}
#include <Plugins/Animation/Pose/Pose.inl>

#endif // POSED_SKELETON_HPP_
