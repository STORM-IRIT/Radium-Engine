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
        Engine::Entity* e = m_engine->getEntityManager()->getOrCreateEntity(data.name);
        e->setTransform(data.transform);

        FancyMeshComponent* component =
                static_cast<FancyMeshComponent*>(addComponentToEntity(e));

        component->handleMeshLoading(data);
	}
}

Engine::Component* Engine::FancyMeshSystem::addComponentToEntity( Engine::Entity* entity )
{
    uint componentId = entity->getComponents().size();

    std::string componentName = "FancyMeshComponent_" + entity->getName() + std::to_string(componentId++);
    FancyMeshComponent* component = new FancyMeshComponent(componentName);

    component->setEntity(entity);
    component->setDrawableManager(m_engine->getDrawableManager());

    entity->addComponent(component);
    this->addComponent(component);

    component->initialize();

    return component;
}

void Engine::FancyMeshSystem::generateTasks(Core::TaskQueue * taskQueue, const Engine::FrameInfo & frameInfo)
{
    // Do nothing, as this system only displays meshes.
}

Engine::FancyMeshComponent * Engine::FancyMeshSystem::addDisplayMeshToEntity(Engine::Entity * entity, const Core::TriangleMesh & mesh)
{
    FancyMeshComponent* comp = static_cast<FancyMeshComponent*> (addComponentToEntity(entity));
    comp->addMeshDrawable(mesh, "Mesh Drawable");
    return comp;
}

}
