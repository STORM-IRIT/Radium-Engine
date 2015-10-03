#include <Plugins/Animation/Pose/Pose.hpp>
namespace AnimationPlugin
{
    Pose::Pose(Pose::Mode mode, const Skeleton* skel, const RawPose& pose) : m_skel(skel)
    {
        m_poses[LOCAL].resize(m_skel->getNumBones());
        m_poses[MODEL].resize(m_skel->getNumBones());
        switch (mode)
        {
            case LOCAL:
                setPose<LOCAL>(pose);
                break;
            case MODEL:
                setPose<MODEL>(pose);
                break;
            default:
                CORE_ASSERT(false,"Should not get there");
        }
    }

    void Pose::recomputeLocal()
    {
        check();
        for (uint b = 0; b < m_skel->getNumBones(); ++b)
        {
            Ra::Core::Transform parentTr = Ra::Core::Transform::Identity();
            int parentIdx = m_skel->getParentIdx(b);
            if (parentIdx != -1)
            {
                parentTr = m_poses[MODEL][parentIdx];
            }
            m_poses[LOCAL][b] = parentTr.inverse() * m_poses[MODEL][b];
        }
    }

    void Pose::recomputeModel()
    {
        for (uint b = 0; b < m_skel->getNumBones(); ++b)
        {
            // We can assume the parent pose is correct at this stage
            // since parents are always before their child bones.
            Ra::Core::Transform parentTr = Ra::Core::Transform::Identity();
            int parentIdx = m_skel->getParentIdx(b);
            if (parentIdx != -1)
            {
                parentTr = m_poses[MODEL][parentIdx];
            }
            m_poses[MODEL][b] = parentTr * m_poses[LOCAL][b];
        }
    }

    void Pose::check() const
    {
        assert(m_poses[MODEL].size() == m_poses[LOCAL].size());
        assert(m_poses[MODEL].size() == unsigned(m_skel->getNumBones()));
    }
}
