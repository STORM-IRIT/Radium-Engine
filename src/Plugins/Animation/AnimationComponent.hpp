#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Engine/Entity/Component.hpp>

#include <Plugins/Animation/Pose/Pose.hpp>

namespace AnimationPlugin
{

class SkeletonBoneRenderObject;

class ANIM_PLUGIN_API AnimationComponent : public Ra::Engine::Component 
{
public:
    AnimationComponent(const std::string& name, Skeleton* skel) : Component(name) { m_skel.reset(skel); }
    virtual ~AnimationComponent() {}

    virtual void initialize() override;

    const Skeleton& getSkeleton() const { return *m_skel; }

protected:
    std::unique_ptr<Pose> m_currentPose;
    std::shared_ptr<Skeleton> m_skel;

    std::vector<SkeletonBoneRenderObject*> m_boneDrawables;

};

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
