 #include <Engine/RadiumEngine.hpp>

#include <thread>
#include <chrono>
#include <mutex>
#include <cstdio>
#include <iostream>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Event/EventEnums.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <Engine/Entity/System.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshSystem.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshComponent.hpp>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>

namespace Ra
{

Engine::RadiumEngine::RadiumEngine()
    : m_quit(false)
{
//    LOG(INFO) << "Engine starting...";
}

Engine::RadiumEngine::~RadiumEngine()
{
}

void Engine::RadiumEngine::initialize()
{
    m_renderObjectManager.reset(new RenderObjectManager);
    m_entityManager.reset(new EntityManager);

    // FIXME(Charly): FancyMeshSystem should not be initialized here.
    FancyMeshSystem* system = new FancyMeshSystem(this);
    m_systems["FancyMeshSystem"] = std::shared_ptr<FancyMeshSystem>(system);
}

void Engine::RadiumEngine::setupScene()
{
    ShaderConfiguration shader("BlinnPhong", "../Shaders");

    // Code to add a cube.
    Material* m0 = new Material("m0");
    m0->setKd(Core::Color(1.0, 0.0, 0.0, 1.0));
    m0->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    RenderTechnique* r0 = new RenderTechnique;
    r0->shaderConfig = shader;
    r0->material = m0;

//    m0->setDefaultShaderProgram(ShaderConfiguration("BlinnPhong", "../Shaders"));

    Material* m1 = new Material("m1");
    m1->setKd(Core::Color(0.0, 1.0, 0.0, 1.0));
    m1->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    RenderTechnique* r1 = new RenderTechnique;
    r1->shaderConfig = shader;
    r1->material = m1;
//    m1->setDefaultShaderProgram(ShaderConfiguration("BlinnPhong", "../Shaders"));

    Material* m2 = new Material("m2");
    m2->setKd(Core::Color(0.0, 0.0, 1.0, 1.0));
    m2->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    RenderTechnique* r2 = new RenderTechnique;
    r2->shaderConfig = shader;
    r2->material = m2;

    Material* m3 = new Material("m3");
    m3->setKd(Core::Color(1.0, 0.0, 1.0, 1.0));
    m3->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    RenderTechnique* r3 = new RenderTechnique;
    r3->shaderConfig = shader;
    r3->material = m3;

    Material* m4 = new Material("m4");
    m4->setKd(Core::Color(1.0, 1.0, 0.0, 1.0));
    m4->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    RenderTechnique* r4 = new RenderTechnique;
    r4->shaderConfig = shader;
    r4->material = m4;

    Material* m5 = new Material("m5");
    m5->setKd(Core::Color(0.0, 1.0, 1.0, 1.0));
    m5->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    RenderTechnique* r5 = new RenderTechnique;
    r5->shaderConfig = shader;
    r5->material = m5;
//    m2->setDefaultShaderProgram(ShaderConfiguration("BlinnPhong", "../Shaders"));

    Core::Transform transform;

    Entity* ent0 = m_entityManager->getOrCreateEntity("box0");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(ent0, Core::MeshUtils::makeBox(), r0);

    transform.setIdentity();
    transform.translation() = Core::Vector3(0, 0, 5);
    ent0->setTransform(transform);

    Entity* ent1 = m_entityManager->getOrCreateEntity("box1");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(ent1, Core::MeshUtils::makeBox(), r1);

    transform.setIdentity();
    transform.translation() = Core::Vector3(0, 0, -5);
    ent1->setTransform(transform);

    Entity* ent2 = m_entityManager->getOrCreateEntity("box2");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(ent2, Core::MeshUtils::makeBox(), r2);

    transform.setIdentity();
    transform.translation() = Core::Vector3(5, 0, 0);
    ent2->setTransform(transform);

    Entity* ent3 = m_entityManager->getOrCreateEntity("box3");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(ent3, Core::MeshUtils::makeBox(), r3);

    transform.setIdentity();
    transform.translation() = Core::Vector3(-5, 0, 0);
    ent3->setTransform(transform);

    Entity* ent4 = m_entityManager->getOrCreateEntity("box4");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(ent4, Core::MeshUtils::makeBox(), r4);

    transform.setIdentity();
    transform.translation() = Core::Vector3(0, 5, 0);
    ent4->setTransform(transform);

    Entity* ent5 = m_entityManager->getOrCreateEntity("box5");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(ent5, Core::MeshUtils::makeBox(), r5);

    transform.setIdentity();
    transform.translation() = Core::Vector3(0, -5, 0);
    ent5->setTransform(transform);

//    Entity* plane3Entity = m_entityManager->getOrCreateEntity("Plane3");
//    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
//        ->addDisplayMeshToEntity(plane3Entity, Core::MeshUtils::makeZNormalQuad(Core::Vector2(50, 50)), m3);

//    transform.setIdentity();
//    transform.translation() = Core::Vector3(0, 0, -150);
//    plane3Entity->setTransform(transform);

//    Entity* plane4Entity = m_entityManager->getOrCreateEntity("Plane4");
//    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
//        ->addDisplayMeshToEntity(plane4Entity, Core::MeshUtils::makeZNormalQuad(Core::Vector2(5, 5)), m4);

//    transform.setIdentity();
//    transform.translation() = Core::Vector3(0, 0, -40);
//    plane4Entity->setTransform(transform);

}

void Engine::RadiumEngine::cleanup()
{
    for (auto& system : m_systems)
    {
        system.second.reset();
    }

    m_entityManager.reset();
    m_renderObjectManager.reset();
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

Engine::RenderObjectManager* Engine::RadiumEngine::getRenderObjectManager() const
{
    return m_renderObjectManager.get();
}

Engine::EntityManager* Engine::RadiumEngine::getEntityManager() const
{
    return m_entityManager.get();
}

} // namespace RadiumEngine
