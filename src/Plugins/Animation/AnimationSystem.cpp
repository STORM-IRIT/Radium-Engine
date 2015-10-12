#include <Plugins/Animation/AnimationSystem.hpp>
#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>
#include <Plugins/Animation/AnimationLoader.hpp>
#include <Plugins/Animation/AnimationComponent.hpp>
#include <string>

namespace AnimationPlugin
{

    void AnimationSystem::generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo)
    {

    }

    Ra::Engine::Component* AnimationSystem::addComponentToEntityInternal(Ra::Engine::Entity* entity, uint id)
    {
		std::string componentName = "AnimationComponent_" + entity->getName() + std::to_string(id);
        AnimationComponent* component = new AnimationComponent(componentName);

        return component;
    }
	
	void AnimationSystem::handleFileLoading(Ra::Engine::Entity *entity, const std::string &filename)
	{
		LOG( logDEBUG ) << "AnimationSystem : loading the file " << filename << "...";
		
		AnimationLoader::AnimationData componentData = AnimationLoader::loadFile(filename);

        AnimationComponent* component = static_cast<AnimationComponent*>(addComponentToEntity(entity));
        component->handleLoading(componentData);
	}
}
