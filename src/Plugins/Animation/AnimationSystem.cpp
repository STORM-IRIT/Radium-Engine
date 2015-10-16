#include <Plugins/Animation/AnimationSystem.hpp>
#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>
#include <Plugins/Animation/AnimationLoader.hpp>
#include <Plugins/Animation/AnimationComponent.hpp>
#include <Plugins/Animation/AnimatorTask.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <string>
#include <iostream>

namespace AnimationPlugin
{
    void AnimationSystem::generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo)
    {
		Scalar currentDelta = m_isPlaying ? frameInfo.m_dt : 0;
		
		for (auto compEntry : this->m_components)
		{
			AnimationComponent* component = std::static_pointer_cast<AnimationComponent>(compEntry.second).get();
			AnimatorTask* task = new AnimatorTask(component, currentDelta);
			taskQueue->registerTask( task );
		}
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
	
	void AnimationSystem::setPlaying(bool isPlaying)
	{
		m_isPlaying = isPlaying;
	}
}
