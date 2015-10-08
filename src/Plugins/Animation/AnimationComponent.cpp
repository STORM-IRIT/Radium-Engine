#include <Plugins/Animation/AnimationComponent.hpp>

#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>
namespace AnimationPlugin {
void AnimationComponent::initialize()
{
    for (uint i = 0; i < m_skel->getNumBones(); ++i)
    {
        if (!m_skel->isLeaf(i))
        {
            m_boneDrawables.push_back(new SkeletonBoneRenderObject(
                m_skel->getName() + " bone " + std::to_string(i), this, i));
            getRoMgr()->addRenderObject(m_boneDrawables.back());
        }
    }
}

    const Pose* AnimationComponent::getPose() const
    {
        return m_currentPose.get();
    }
}

