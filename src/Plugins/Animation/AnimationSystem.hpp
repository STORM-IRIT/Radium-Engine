#ifndef ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
#define ANIMPLUGIN_ANIMATION_SYSTEM_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Engine/Entity/System.hpp>

namespace AnimationPlugin
{
    class AnimationSystem :  public Ra::Engine::System
    {
    public:
        virtual void generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo);
        void initialize( ) override {}

    protected:
        virtual Ra::Engine::Component* addComponentToEntityInternal(Ra::Engine::Entity* entity, uint id);

    };

}


#endif // ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
