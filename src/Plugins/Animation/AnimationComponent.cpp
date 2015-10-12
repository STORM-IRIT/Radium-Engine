#include <Plugins/Animation/AnimationComponent.hpp>
#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>
#include <assimp/scene.h>
#include <iostream>

namespace AnimationPlugin
{
	void recursiveSkeletonRead(const aiNode* node, const aiScene* scene);	

	void AnimationComponent::initialize()
	{
		for (uint i = 0; i < m_skel->m_hier.size(); ++i)
		{
			if (!m_skel->m_hier.isLeaf(i))
			{
				m_boneDrawables.push_back(new SkeletonBoneRenderObject(m_skel->getName() + " bone " + std::to_string(i), this, i));
				getRoMgr()->addRenderObject(m_boneDrawables.back());
			}
		}
	}
	
	const Ra::Core::Animation::Pose* AnimationComponent::getPose() const
	{
		return m_currentPose.get();
	}
	
	void AnimationComponent::set(Ra::Core::Animation::Skeleton *skel, const Ra::Core::Animation::RefPose &refPose)
	{
		m_refPose = refPose;
		m_skel.reset(skel);
        //m_currentPose.reset(new Pose(Pose::MODEL, m_skel.get(), refPose));
		// TODO
	}
	
	void AnimationComponent::handleLoading(const AnimationLoader::AnimationData& data)
	{
		LOG( logDEBUG ) << "Animation component: loading a skeleton";	

	}
}
