#include <Plugins/Animation/AnimationComponent.hpp>

#include <assimp/scene.h>
#include <iostream>

#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>
namespace AnimationPlugin 
{
	void recursiveSkeletonRead(const aiNode* node, const aiScene* scene);
	
    void AnimationComponent::initialize()
    {
        for (uint i = 0; i < m_skel.size(); ++i)
        {
            if (!m_skel.m_hier.isLeaf(i))
            {
                m_boneDrawables.push_back(new SkeletonBoneRenderObject(
                m_skel.getName() + " bone " + std::to_string(i), this, i));
                getRoMgr()->addRenderObject(m_boneDrawables.back());
            }
        }
    }

    void AnimationComponent::getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const
    {
        for (uint i = 0; i < m_skel.size(); ++i)
        {
             const Ra::Core::Transform& tr = m_skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL)[i];
             propsOut.push_back(Ra::Engine::EditableProperty(tr, std::string("Transform of") + m_skel.getLabel(i)));
        }
    }

    void AnimationComponent::set(const Ra::Core::Animation::Skeleton& skel)
	{
        m_skel = skel;
        m_refPose = skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL);
	}
	
	void AnimationComponent::handleLoading(const AnimationLoader::AnimationData& data)
	{
		LOG( logDEBUG ) << "Animation component: loading a skeleton";
		
	}
	
	void recursiveSkeletonRead(const aiNode* node, const aiScene* scene)
	{
    // FIXME	std::cout << aiNode->mNumMeshes << std::endl;
	}
}
