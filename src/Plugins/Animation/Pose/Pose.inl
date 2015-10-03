#include "Pose.hpp"
namespace AnimationPlugin
{
    template<Pose::Mode MODE>
    inline void Pose::setPose(const RawPose& pose)
    {
        m_poses[MODE] = pose;
        switch (MODE)
        {
            case LOCAL:
                recomputeModel();
                break;
            case MODEL:
                recomputeLocal();
                break;
            default:
                assert(false); // Wrong mode. We should never get here.
        }
        check();
    }

    template<Pose::Mode MODE>
    inline const Pose::RawPose& Pose::getPose() const
    {
        return m_poses[MODE];
    }

    template<Pose::Mode MODE>
    inline const Ra::Core::Transform& Pose::getBoneTransform(int boneIdx) const
    {
        CORE_ASSERT(boneIdx >= 0 && boneIdx < m_skel->getNumBones(), "Invalid bone index");
        return m_poses[MODE][boneIdx];
    }

    inline const Skeleton* Pose::getSkeleton() const
    {
        return m_skel;
    }

    void Pose::resetPose()
    {
        setPose<MODEL>(m_skel->getRefPose());
    }

    template<Pose::Mode MODE>
    void Pose::getRelativePose( RawPose& relPoseOut,
                                 const Pose* poseStart,
                                 const Pose* poseEnd)
    {
        CORE_ASSERT(poseStart->m_skel == poseEnd->m_skel, "Pose do not belong to the same skeleton.");
        const RawPose& start = poseStart->m_poses[MODE];
        const RawPose& end = poseEnd->m_poses[MODE];

        const uint numBones = poseStart->m_skel->getNumBones();
        relPoseOut.clear();
        relPoseOut.reserve(numBones);
        for (uint i = 0; i < numBones; ++i)
        {
            relPoseOut.push_back(end[i] * start[i].inverse());
        }
    }

    template<Pose::Mode MODE>
    void Pose::applyRelativePose(const RawPose& relPose)
    {
        // FIXME (Val)!!
        assert(false);
    }


    void Pose::setBoneTransform(int boneIdx, const Ra::Core::Transform& tr)
    {
        CORE_ASSERT(boneIdx >= 0 && uint(boneIdx) < m_skel->getNumBones(), "Invalid bone index.");
        m_poses[LOCAL][boneIdx] = tr;
        recomputeModel();
    }


    void Pose::applyRelativeBoneTransform(int boneIdx, const Ra::Core::Transform& tr)
    {
//        Ra::Core::Matrix3 rot = m_poses[LOCAL][bone_idx].rotation();

        // FIXME(Val)

        //m_poses[LOCAL][bone_idx].rotation() = (rot * tr.rotation()).eval();

        recomputeModel();
    }

    void Pose::applyAbsoluteBoneTransform(int boneIdx, const Ra::Core::Transform& tr)
    {
        //m_poses[LOCAL][bone_idx].rotation() = tr.rotation();

        // FIXME(Val)

        recomputeModel();
    }
}