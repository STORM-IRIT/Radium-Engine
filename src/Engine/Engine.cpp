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
#include <Engine/Renderer/Mesh.hpp>
#include <Engine/Renderer/DrawableComponent.hpp>

namespace Ra
{

RadiumEngine::RadiumEngine()
    : m_quit(false)
    , m_angle(0.0)
{
}

void RadiumEngine::initialize()
{
    System* renderSystem = new ForwardRenderer;
    renderSystem->initialize();

    m_systems["RenderSystem"] = std::shared_ptr<System>(renderSystem);
}

void RadiumEngine::setupScene()
{
    ComponentManager* cManager = ComponentManager::createInstance();
    EntityManager* eManager = EntityManager::createInstance();

    Mesh* mesh = new Mesh("Mesh");
    VertexData v0, v1, v2;
    v0.position = Core::Vector3(-0.5, -0.5, 0);
    v1.position = Core::Vector3(0, 0.5, 0);
    v2.position = Core::Vector3(0.5, -0.5, 0);
    MeshData d;
    d.vertices = {v0, v1, v2};
    d.indices  = {0, 2, 1};
    mesh->loadGeometry(d);

    m_entity = eManager->createEntity();
    DrawableComponent* comp = new DrawableComponent();
    comp->addDrawable(mesh);

    cManager->addComponent(comp);
    m_entity->addComponent(comp);

    m_systems["RenderSystem"]->addComponent(comp);
}

void RadiumEngine::start()
{

    std::thread t(&RadiumEngine::run, this);
    t.detach();
}

void RadiumEngine::run()
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

void RadiumEngine::cleanup()
{
    for (auto& system : m_systems)
    {
        system.second.reset();
    }
}

void RadiumEngine::quit()
{
    std::lock_guard<std::mutex> lock(m_quitMutex);
    m_quit = true;
}

bool RadiumEngine::quitRequested()
{
    bool quit;
    std::lock_guard<std::mutex> lock(m_quitMutex);
    quit = m_quit;
    return quit;
}

System* RadiumEngine::getSystem(const std::string& system) const
{
    System* sys = nullptr;
    auto it = m_systems.find(system);

    if (it != m_systems.end())
    {
        sys = it->second.get();
    }

    return sys;
}

} // namespace RadiumEngine
