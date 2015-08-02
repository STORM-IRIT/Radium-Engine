#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderQueue/RenderQueue.hpp>

namespace Ra
{

Engine::RenderObject::RenderObject(const std::string& name)
	: IndexedObject()
	, m_name(name)
    , m_type(OPAQUE)
    , m_shader(nullptr)
    , m_material(nullptr)
    , m_mesh(nullptr)
	, m_isDirty(true)
{
}

Engine::RenderObject::~RenderObject()
{
}

void Engine::RenderObject::updateGL()
{
	// Do not update while we are cloning
	std::lock_guard<std::mutex> lock(m_updateMutex);

    if (m_material)
    {
        m_material->updateGL();
    }

    if (m_mesh)
    {
        m_mesh->updateGL();
    }

    m_isDirty = false;
}

void Engine::RenderObject::feedRenderQueue(RenderQueue* queue)
{

}

Engine::RenderObject* Engine::RenderObject::clone()
{
	// Do not clone while we are updating GL internals
	std::lock_guard<std::mutex> lock(m_updateMutex);

    RenderObject* newRO = new RenderObject(m_name);

    newRO->setRenderObjectType(m_type);
    newRO->setShader(m_shader);
    newRO->setMaterial(m_material);
    newRO->setVisible(m_visible);
    newRO->setComponent(m_component);

    if (m_mesh)
    {
        Mesh* newMesh = new Mesh(m_mesh->getName());
        newMesh->loadGeometry(m_mesh->getMeshData(),
                              m_mesh->getTangents(),
                              m_mesh->getBitangents(),
                              m_mesh->getTexcoords());
        newRO->setMesh(newMesh);
    }

    return newRO;
}

}
