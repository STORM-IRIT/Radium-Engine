#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP _
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP 

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Engine/Entity/Component.hpp>


#include <Plugins/Animation/Pose/Pose.hpp>

namespace AnimationPlugin
{

class AnimationComponent : public Ra::Engine::Component 
{
public:
    AnimationComponent(const std::string& name);
    virtual ~AnimationComponent();

    virtual void initialize() override;

protected:
    std::unique_ptr<Pose> m_currentPose;
    std::shared_ptr<Skeleton> m_skel;

};

}



#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP
