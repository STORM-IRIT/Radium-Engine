#if 0
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshRenderObject.hpp>

#include <cstdio>
#include <iostream>

#include <Core/CoreMacros.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{

Engine::FancyMeshRenderObject::FancyMeshRenderObject(const std::string& name)
    : Engine::RenderObject(name)
{
}

Engine::FancyMeshRenderObject::~FancyMeshRenderObject()
{
    for (auto& mesh : m_meshes)
    {
        // Keep sure only this meshComponent has ownership on its meshs
        CORE_ASSERT(mesh.second.unique(), "Non-unique mesh about to be removed.");
        mesh.second.reset();
    }

    m_meshes.clear();
}

void Engine::FancyMeshRenderObject::draw(const Core::Matrix4& viewMatrix,
                                     const Core::Matrix4& projMatrix,
                                     ShaderProgram *shader)
{
    Core::Matrix4 modelMatrix = m_component->getEntity()->getTransformAsMatrix();
    Core::Matrix4 mvp = projMatrix * viewMatrix * modelMatrix;

    shader->setUniform("model", modelMatrix);
    shader->setUniform("view", viewMatrix);
    shader->setUniform("proj", projMatrix);
    shader->setUniform("mvp", mvp);

    m_material->bind(shader);

    for (const auto& mesh : m_meshes)
    {
        mesh.second->render();
    }
}

void Engine::FancyMeshRenderObject::draw(const Core::Matrix4& viewMatrix,
                                     const Core::Matrix4& projMatrix,
                                     Light* light)
{
    // TODO(Charly): Potential optimization
    //                  -> Keep sure all materials are ordered.
    if (nullptr == m_material)
    {
        return;
    }


    ShaderProgram* shader = m_material->getCurrentShaderProgram();
    if (nullptr == shader)
    {
        return;
    }

    shader->bind();

    Core::Matrix4 modelMatrix = m_component->getEntity()->getTransformAsMatrix();
    Core::Matrix4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;
    // TODO(Charly): Add normal matrix

    shader->setUniform("model", modelMatrix);
    shader->setUniform("view", viewMatrix);
    shader->setUniform("proj", projMatrix);
    shader->setUniform("mvp", mvpMatrix);

    m_material->bind();
    light->bind(shader);

    for (const auto& mesh : m_meshes)
    {
        mesh.second->render();
    }
}

void Engine::FancyMeshRenderObject::addMesh(Mesh* mesh)
{
    std::string name = mesh->getName();

    if (m_meshes.find(name) != m_meshes.end())
    {
        LOG(DEBUG) << "Warning, mesh " << name << " has already been added.";
        return;
    }

    m_meshes.insert(MeshByName(name, std::shared_ptr<Mesh>(mesh)));

    recomputeBbox();
}

void Engine::FancyMeshRenderObject::removeMesh(const std::string& name)
{
    auto mesh = m_meshes.find(name);
    if (mesh == m_meshes.end())
    {
        LOG(DEBUG) << "Warning, " << name << " does not exist.";
        return;
    }

    mesh->second.reset();
    m_meshes.erase(mesh);

    recomputeBbox();
}

void Engine::FancyMeshRenderObject::removeMesh(Engine::Mesh* mesh)
{
    removeMesh(mesh->getName());
}

Engine::Mesh* Engine::FancyMeshRenderObject::getMesh(const std::string &name) const
{
    auto mesh = m_meshes.find(name);
    Engine::Mesh* ret = nullptr;

    if (mesh != m_meshes.end())
    {
        ret = mesh->second.get();
    }

    return ret;
}

void Engine::FancyMeshRenderObject::updateGLInternal()
{
    CORE_ASSERT(m_material, "No material");
    m_material->updateGL();
    for (auto& mesh : m_meshes)
    {
        mesh.second->updateGL();
    }

    m_isDirty = false;
}

Engine::RenderObject* Engine::FancyMeshRenderObject::cloneInternal()
{
    // FIXME(Charly): Did I forget to clone something ?
    FancyMeshRenderObject* renderObject = new FancyMeshRenderObject(m_name);

    for (const auto& it : m_meshes)
    {
        Mesh* oldMesh = getMesh(it.first);
        Mesh* newMesh = new Mesh(oldMesh->getName());
        newMesh->loadGeometry(oldMesh->getMeshData(),
                              oldMesh->getTangents(),
                              oldMesh->getBitangents(),
                              oldMesh->getTexcoords());

        renderObject->addMesh(newMesh);
    }

    // FancyMesh materials do not require particular attention,
    // just share the pointer.
    renderObject->setMaterial(m_material);
    renderObject->setVisible(m_visible);
    renderObject->setComponent(m_component);

    return renderObject;
}

void Engine::FancyMeshRenderObject::recomputeBbox()
{
    m_boundingBox.setEmpty();

    for (const auto& mesh : m_meshes)
    {
        m_boundingBox.extend(Core::MeshUtils::getAabb(mesh.second->getMeshData()));
    }
}

//void Engine::FancyMeshRenderObject::setSelected(bool selected)
//{
//    m_isSelected = selected;

//    if (selected)
//    {
//        m_material->changeMode(Material::MODE_CONTOUR);
//    }
//    else
//    {
//        m_material->changeMode(Material::MODE_DEFAULT);
//    }
//}

} // namespace Ra
#endif
