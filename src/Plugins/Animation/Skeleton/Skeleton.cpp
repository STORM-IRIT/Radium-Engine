#include <Plugins/Animation/Skeleton/Skeleton.hpp>
namespace AnimationPlugin
{
    Skeleton::Skeleton(const std::string& name)
            : m_name(name)
    {
    }

    uint Skeleton::getNumBones() const
    {
        return m_bones.size();
    }
    int Skeleton::getParentIdx(int boneIdx) const
    {

        CORE_ASSERT(boneIdx >= 0 && unsigned(boneIdx) < m_bones.size(), "Invalid bone index.");
        CORE_ASSERT(m_parents[boneIdx] < boneIdx, "Invalid parent ordering.");
        return m_parents[boneIdx];
    }

    const Bone& Skeleton::getBone(int boneIdx) const
    {
        CORE_ASSERT(boneIdx >= 0 && unsigned(boneIdx) < m_bones.size(), "Invalid bone index.");
        return m_bones[boneIdx];
    }

    const std::vector <Ra::Core::Transform>& Skeleton::getRefPose() const
    {
        return m_refPose;
    }

    const std::vector<int>& Skeleton::getChildrenIdx(int boneIdx) const
    {
        CORE_ASSERT(boneIdx >= 0 && unsigned(boneIdx) < m_bones.size(), "Invalid bone index.");
        return m_children[boneIdx];
    }

    bool Skeleton::isLeaf(int boneIdx) const
    {
        CORE_ASSERT(boneIdx >= 0 && unsigned(boneIdx) < m_bones.size(), "Invalid bone index.");
        return m_children[boneIdx].empty();
    }

    const std::string& Skeleton::getName() const
    {
        return m_name;
    }

    void Skeleton::addBone(const Bone& bone, int parentIdx)
    {
        // Parent index can be -1.
        CORE_ASSERT(parentIdx >= -1 && parentIdx < int(m_bones.size()), "Parent index needs to be a positive int within the existing bones or -1");
        int idx = int(m_bones.size());
        m_bones.push_back(bone);
        m_parents.push_back(parentIdx);
        m_children.push_back(std::vector<int>());
        if (parentIdx >= 0)
        {
            m_children[parentIdx].push_back(idx);
        }
        check();
    }

    void Skeleton::setRefPose(const std::vector <Ra::Core::Transform>& pose)
    {
        CORE_ASSERT(pose.size() == unsigned(getNumBones()), "Incompatible pose");
        m_refPose = pose;
    }

    void Skeleton::check() const
    {
        CORE_ASSERT(m_bones.size() == m_parents.size(), "Bone list and parent list are incompatible.");
        // checks no bone is there before its parent.
        for (int b = 0; b < getNumBones(); ++b)
        {
            CORE_ASSERT(getParentIdx(b) < b, "Parent ordering is wrong.");
            CORE_ASSERT(getParentIdx(b) >= -1, "Parent index is wrong");
            for (auto c : m_children[b])
            {
                CORE_ASSERT(getParentIdx(c) == b, " Inconsistent parent indexes");
            }
            CORE_ASSERT(isLeaf(b) == (m_children[b].size() == 0), " Only leaves should have no children.");
        }
    }
}
