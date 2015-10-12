#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Engine/Entity/Component.hpp>

#include <Plugins/Animation/Pose/Pose.hpp>
#include <Plugins/Animation/AnimationLoader.hpp>

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
	void set(Skeleton* skel, const RawPose& refPose);
	void handleLoading(const AnimationLoader::AnimationData& data);
	
    const Skeleton& getSkeleton() const { return *m_skel; }

    const Pose* getPose() const;

protected:
    std::unique_ptr<Pose> m_currentPose;
    std::shared_ptr<Skeleton> m_skel;
    RawPose m_refPose; // Ref pose in model space.

    std::vector<SkeletonBoneRenderObject*> m_boneDrawables;

};

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
