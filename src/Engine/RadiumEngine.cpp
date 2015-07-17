#include <Engine/RadiumEngine.hpp>

#include <thread>
#include <chrono>
#include <mutex>
#include <cstdio>

#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/System.hpp>
#include <Engine/Entity/ComponentManager.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/EntityManager.hpp>
#include <Engine/Renderer/ForwardRenderer.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Mesh/MeshLoader.hpp>
#include <Engine/Renderer/Drawable/DrawableComponent.hpp>

namespace Ra
{

Engine::RadiumEngine::RadiumEngine()
    : m_quit(false)
{
}

void Engine::RadiumEngine::initialize()
{
    Engine::System* renderSystem = new Engine::ForwardRenderer;
    renderSystem->initialize();

    m_systems["RenderSystem"] = std::shared_ptr<Engine::System>(renderSystem);
}

void Engine::RadiumEngine::setupScene()
{
    m_componentManager = ComponentManager::createInstance();
    m_entityManager    = EntityManager::createInstance();
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

//        m_angle += 0.05;
//        m_entity->setTransform(Core::Transform(Core::AngleAxis(std::sin(m_angle),
//                                                   Core::Vector3(0.0, 1.0, 0.0))));
    }

    cleanup();
}

void Engine::RadiumEngine::cleanup()
{
    for (auto& system : m_systems)
    {
        system.second.reset();
    }
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

void Engine::RadiumEngine::loadFile(const std::string& file)
{
    MeshLoader::loadFile(file, this);
}

} // namespace RadiumEngine
