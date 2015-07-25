#include <Engine/Renderer/FancyMeshPlugin/FancyMeshSystem.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshComponent.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoader.hpp>

namespace Ra
{

Engine::FancyMeshSystem::FancyMeshSystem(RadiumEngine* engine)
	: System(engine)
{
}

Engine::FancyMeshSystem::~FancyMeshSystem()
{
}

void Engine::FancyMeshSystem::initialize()
{
}

void Engine::FancyMeshSystem::update(Scalar dt)
{
	for (auto& c : m_components)
	{
		c.second->update(dt);
	}
}

void Engine::FancyMeshSystem::handleFileLoading(const std::string& filename)
{
	// TODO
	DrawableManager* drawableManager = m_engine->getDrawableManager();
	EntityManager* entityManager = m_engine->getEntityManager();
	ComponentManager* componentManager = m_engine->getComponentManager();

	printf("FancyMeshSystem::Loading file %s\n", filename.c_str());

	std::vector<FancyComponentData> componentsData = FancyMeshLoader::loadFile(filename);

	printf("Found %u components to create\n", componentsData.size());

	for (uint i = 0; i < componentsData.size(); ++i)
	{
		FancyComponentData data = componentsData[i];

		// Retrieve entity if exist, create it otherwise
		Entity* entity;
		if (entityManager->entityExists(data.name))
		{
			entity = entityManager->getEntity(data.name);
		}
		else
		{
			entity = entityManager->createEntity(data.name);
		}

		entity->setTransform(data.transform);

		FancyMeshComponent* component = new FancyMeshComponent;
		componentManager->addComponent(component);

		component->setSystem(this);
		component->setEntity(entity);
		component->setDrawableManager(drawableManager);

		entity->addComponent(component);
		addComponent(component);

		component->initialize();

		component->handleMeshLoading(data);
	}
}

}