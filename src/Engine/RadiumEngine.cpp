 #include <Engine/RadiumEngine.hpp>

#include <thread>
#include <chrono>
#include <mutex>
#include <cstdio>
#include <iostream>

#include <Core/String/StringUtils.hpp>
#include <Core/Event/EventEnums.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <Engine/Entity/System.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Mesh/MeshLoader.hpp>

#include <Engine/Renderer/FancyMeshPlugin/FancyMeshSystem.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshComponent.hpp>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>

namespace Ra
{

Engine::RadiumEngine::RadiumEngine()
    : m_quit(false)
{
}

Engine::RadiumEngine::~RadiumEngine()
{
}

void Engine::RadiumEngine::initialize()
{
	m_drawableManager.reset(new DrawableManager);
	m_entityManager.reset(new EntityManager);

    // FIXME(Charly): FancyMeshSystem should not be initialized here.
	FancyMeshSystem* system = new FancyMeshSystem(this);
	m_systems["FancyMeshSystem"] = std::shared_ptr<FancyMeshSystem>(system);
}


void Engine::RadiumEngine::setupScene()
{
    // Code to add a cube.
    Entity * cubeEntity = m_entityManager->getOrCreateEntity("Cube");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(cubeEntity,Core::MeshUtils::makeBox());
}

void Engine::RadiumEngine::cleanup()
{
    for (auto& system : m_systems)
    {
        system.second.reset();
    }

	m_entityManager.reset();
    m_drawableManager.reset();
}

void Engine::RadiumEngine::getTasks(Core::TaskQueue* taskQueue,  Scalar dt)
{
    FrameInfo frameInfo;
    frameInfo.m_dt = dt;
    for (auto& syst : m_systems)
    {
        syst.second->generateTasks(taskQueue, frameInfo);
    }
}

Engine::System* Engine::RadiumEngine::getSystem(const std::string& system) const
{
    Engine::System* sys = nullptr;
    auto it = m_systems.find(system);

    if (it != m_systems.end())
    {
        sys = it->second.get();
    }

    return sys;
}

bool Engine::RadiumEngine::loadFile(const std::string& file)
{
	for (auto& system : m_systems)
	{
		system.second->handleFileLoading(file);
	}

    return true;
}

bool Engine::RadiumEngine::handleKeyEvent(const Core::KeyEvent &event)
{
    for (const auto& system : m_systems)
    {
        if (system.second->handleKeyEvent(event))
        {
            return true;
        }
    }

    return false;
}

bool Engine::RadiumEngine::handleMouseEvent(const Core::MouseEvent &event)
{
    for (const auto& system : m_systems)
    {
        if (system.second->handleMouseEvent(event))
        {
            return true;
        }
    }

    return false;
}

Engine::DrawableManager* Engine::RadiumEngine::getDrawableManager() const
{
	return m_drawableManager.get(); 
}

Engine::EntityManager* Engine::RadiumEngine::getEntityManager() const
{
	return m_entityManager.get(); 
}

} // namespace RadiumEngine
