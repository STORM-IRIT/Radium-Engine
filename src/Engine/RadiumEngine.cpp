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
#include <Engine/Entity/System.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Mesh/MeshLoader.hpp>

namespace Ra
{

Engine::RadiumEngine::RadiumEngine()
    : m_quit(false)
{
}

void Engine::RadiumEngine::initialize()
{
	m_drawableManager.reset(new DrawableManager);
	m_componentManager.reset(new ComponentManager);
	m_entityManager.reset(new EntityManager);
}

void Engine::RadiumEngine::setupScene()
{
}

void Engine::RadiumEngine::start()
{
    std::thread t(&RadiumEngine::run, this);
    t.detach();
}

void Engine::RadiumEngine::run()
{
    while (!quitRequested())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    cleanup();
}

void Engine::RadiumEngine::cleanup()
{
    for (auto& system : m_systems)
    {
        system.second.reset();
    }

	m_drawableManager.reset();
	m_componentManager.reset();
	m_entityManager.reset();
}

void Engine::RadiumEngine::quit()
{
    std::lock_guard<std::mutex> lock(m_quitMutex);
    m_quit = true;
}

bool Engine::RadiumEngine::quitRequested()
{
    bool quit;
    std::lock_guard<std::mutex> lock(m_quitMutex);
    quit = m_quit;
    return quit;
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

Engine::Entity* Engine::RadiumEngine::createEntity()
{
    std::lock_guard<std::mutex> lock(m_managersMutex);
    return m_entityManager->createEntity();
}

void Engine::RadiumEngine::addComponent(Component* component,
                                        Entity* entity,
                                        const std::string& system)
{
    std::string err;
    Core::StringUtils::stringPrintf(err, "System \"%s\" : No such system.\n", system.c_str());

    std::lock_guard<std::mutex> lock(m_managersMutex);
    CORE_ASSERT(m_systems.find(system) != m_systems.end(), err.c_str());

    m_componentManager->addComponent(component);
    entity->addComponent(component);
    m_systems[system]->addComponent(component);
}

bool Engine::RadiumEngine::loadFile(const std::string& file)
{
    std::cout << " Engine loading file" << file << std::endl;

	for (auto& system : m_systems)
	{
		system.second->handleFileLoading(file);
	}

    return true;
}

std::vector<Engine::Entity*> Engine::RadiumEngine::getEntities() const
{
    std::lock_guard<std::mutex> lock(m_managersMutex);
    return m_entityManager->getEntities();
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
Engine::ComponentManager* Engine::RadiumEngine::getComponentManager() const
{
	return m_componentManager.get();
}


} // namespace RadiumEngine
