#include <Plugins/Animation/AnimationSystem.hpp>

#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>


namespace AnimationPlugin
{

    void AnimationSystem::generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo)
    {

    }

    Ra::Engine::Component* AnimationSystem::addComponentToEntityInternal(
            Ra::Engine::Entity* entity, uint id)
    {
        return nullptr;
    }
}
