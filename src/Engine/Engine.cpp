#include <Engine/Engine.hpp>

#include <thread>
#include <chrono>
#include <mutex>
#include <cstdio>

#include <Engine/Entity/System.hpp>
#include <Engine/Entity/ComponentManager.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/EntityManager.hpp>
#include <Engine/Renderer/ForwardRenderer.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Drawable/DrawableComponent.hpp>

namespace Ra
{

Engine::RadiumEngine::RadiumEngine()
    : m_quit(false)
    , m_angle(0.0)
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
    Engine::ComponentManager* cManager = Engine::ComponentManager::createInstance();
    Engine::EntityManager* eManager = Engine::EntityManager::createInstance();

    Engine::Mesh* mesh = new Engine::Mesh("Mesh");
    Core::Vector3 v0(-0.5, -0.5, 0);
    Core::Vector3 v1(0, 0.5, 0);
    Core::Vector3 v2 (0.5, -0.5, 0);

    Core::TriangleMesh d;
    d.m_vertices = {v0, v1, v2};
    d.m_triangles= {Core::Vector3i(0, 2, 1)};
    mesh->loadGeometry(d);

    m_entity = eManager->createEntity();
    Engine::DrawableComponent* comp = new Engine::DrawableComponent();
    comp->addDrawable(mesh);

    cManager->addComponent(comp);
    m_entity->addComponent(comp);

    m_systems["RenderSystem"]->addComponent(comp);
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

        m_angle += 0.05;
        m_entity->setTransform(Core::Transform(Core::AngleAxis(std::sin(m_angle),
                                                   Core::Vector3(0.0, 1.0, 0.0))));
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

} // namespace RadiumEngine
