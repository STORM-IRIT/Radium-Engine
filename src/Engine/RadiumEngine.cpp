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
    // Code to add a cube.
    Material* m0 = new Material("Material0");
    m0->setKd(Core::Color(1.0, 0.0, 0.0, 0.75));
    m0->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    m0->setMaterialType(Material::MAT_TRANSPARENT);
//    m0->setDefaultShaderProgram(ShaderConfiguration("BlinnPhong", "../Shaders"));

    Material* m1 = new Material("Material1");
    m1->setKd(Core::Color(1.0, 1.0, 0.0, 0.75));
    m1->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    m1->setMaterialType(Material::MAT_TRANSPARENT);
//    m1->setDefaultShaderProgram(ShaderConfiguration("BlinnPhong", "../Shaders"));

    Material* m2 = new Material("Material2");
    m2->setKd(Core::Color(0.0, 0.0, 1.0, 0.75));
    m2->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
    m2->setMaterialType(Material::MAT_TRANSPARENT);

    Material* m3 = new Material("Opaque0");
    m3->setKd(Core::Color(1.0, 1.0, 1.0, 1.0));
    m3->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));

    Material* m4 = new Material("Opaque1");
    m4->setKd(Core::Color(0.5, 0.5, 0.5, 1.0));
    m4->setKs(Core::Color(0.0, 0.0, 0.0, 1.0));
//    m2->setDefaultShaderProgram(ShaderConfiguration("BlinnPhong", "../Shaders"));

    Core::Transform transform;

    Entity* plane0Entity = m_entityManager->getOrCreateEntity("Plane0");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(plane0Entity, Core::MeshUtils::makeZNormalQuad(Core::Vector2(10, 10)), m0);

    transform.setIdentity();
    transform.translation() = Core::Vector3(0, 0, -70);
    plane0Entity->setTransform(transform);

    Entity* plane1Entity = m_entityManager->getOrCreateEntity("Plane1");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(plane1Entity, Core::MeshUtils::makeZNormalQuad(Core::Vector2(10, 10)), m1);

    transform.setIdentity();
    transform.translation() = Core::Vector3(0, 0, -60);
    plane1Entity->setTransform(transform);

    Entity* plane2Entity = m_entityManager->getOrCreateEntity("Plane2");
    static_cast<FancyMeshSystem*>(m_systems["FancyMeshSystem"].get())
        ->addDisplayMeshToEntity(plane2Entity, Core::MeshUtils::makeZNormalQuad(Core::Vector2(10, 10)), m2);

    transform.setIdentity();
    transform.translation() = Core::Vector3(0, 0, -50);
    plane2Entity->setTransform(transform);

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
