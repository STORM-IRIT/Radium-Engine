#include <Engine/Renderer/FancyMeshPlugin/FancyMeshSystem.hpp>

#include <Core/String/StringUtils.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshComponent.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoader.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

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
	DataVector componentsData = FancyMeshLoader::loadFile(filename);

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
    component->setRenderObjectManager(m_engine->getRenderObjectManager());

    entity->addComponent(component);
    this->addComponent(component);

    component->initialize();

    return component;
}

void Engine::FancyMeshSystem::generateTasks(Core::TaskQueue * taskQueue, const FrameInfo & frameInfo)
{
    // Do nothing, as this system only displays meshes.
    Core::DummyTask* task = new Core::DummyTask;
    Core::DummyTaskParams p; p.m_param= frameInfo.m_dt;
    task->init(&p);
    taskQueue->queueTask(taskQueue->registerTask(task));
}

Engine::FancyMeshComponent * Engine::FancyMeshSystem::addDisplayMeshToEntity(Entity * entity, const Core::TriangleMesh & mesh)
{
    FancyMeshComponent* comp = static_cast<FancyMeshComponent*>(addComponentToEntity(entity));
    comp->addMeshRenderObject(mesh, "Mesh RenderObject");
    return comp;
}

Engine::FancyMeshComponent* Engine::FancyMeshSystem::addDisplayMeshToEntity(Entity *entity,
                                                                            const Core::TriangleMesh &mesh,
                                                                            RenderTechnique* technique)
{
    FancyMeshComponent* comp = static_cast<FancyMeshComponent*>(addComponentToEntity(entity));
    comp->addMeshRenderObject(mesh, "Mesh RenderObject", technique);
    return comp;
}

}
