#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>
#include <Engine/Entity/Component.hpp>

//#include <Plugins/Animation/Pose/Pose.hpp>
#include <Plugins/Animation/AnimationLoader.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

namespace AnimationPlugin
{

class SkeletonBoneRenderObject;

class ANIM_PLUGIN_API AnimationComponent : public Ra::Engine::Component 
{
public:
    AnimationComponent(const std::string& name) : Component(name)
    {
    }
	
    virtual ~AnimationComponent() {}

    virtual void initialize() override;
	void set(Ra::Core::Animation::Skeleton* skel, const Ra::Core::Animation::RefPose& refPose);
	void handleLoading(const AnimationLoader::AnimationData& data);
	
    const Ra::Core::Animation::Skeleton& getSkeleton() const
	{
		return *m_skel;
	}

    const Ra::Core::Animation::Pose* getPose() const;

protected:
    std::unique_ptr<Ra::Core::Animation::Pose> m_currentPose;
    std::shared_ptr<Ra::Core::Animation::Skeleton> m_skel;
    Ra::Core::Animation::RefPose m_refPose; // Ref pose in model space.
    std::vector<SkeletonBoneRenderObject*> m_boneDrawables;

};

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
