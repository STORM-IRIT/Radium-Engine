#include <Engine/Renderer/FancyMeshPlugin/FancyMeshSystem.hpp>

#include <Core/String/StringUtils.hpp>
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
	printf("FancyMeshSystem::Loading file %s\n", filename.c_str());

	std::vector<FancyComponentData> componentsData = FancyMeshLoader::loadFile(filename);

    printf("Found %zu components to create\n", componentsData.size());

	for (uint i = 0; i < componentsData.size(); ++i)
	{
		FancyComponentData data = componentsData[i];

		// Retrieve entity if exist, create it otherwise
        m_engine->getEntityManager()->getOrCreateEntity(data.name)->setTransform(data.transform);

        FancyMeshComponent* component =
                static_cast<FancyMeshComponent*>(createComponent(data.name));

        component->handleMeshLoading(data);
	}
}

Engine::Component* Engine::FancyMeshSystem::createComponent(const std::string &name)
{
    static uint componentId = 0;
    Entity* entity = m_engine->getEntityManager()->getOrCreateEntity(name);

    std::string componentName;
    Core::StringUtils::stringPrintf(componentName, "FancyMeshComponent_%s_u",
                                    name.c_str(), componentId++);
    FancyMeshComponent* component = new FancyMeshComponent(componentName);

    component->setSystem(this);
    component->setEntity(entity);
    component->setDrawableManager(m_engine->getDrawableManager());

    entity->addComponent(component);
    addComponent(component);

    component->initialize();

    return component;
}

}
