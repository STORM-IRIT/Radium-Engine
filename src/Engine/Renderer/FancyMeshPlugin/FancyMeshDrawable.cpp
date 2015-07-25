#include <Engine/Renderer/FancyMeshPlugin/FancyMeshDrawable.hpp>

#include <cstdio>
#include <iostream>

#include <Core/CoreMacros.hpp>
#include <Core/Math/Matrix.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Drawable/Drawable.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{

Engine::FancyMeshDrawable::FancyMeshDrawable(const std::string& name)
    : Engine::Drawable(name)
{
}

Engine::FancyMeshDrawable::~FancyMeshDrawable()
{
    for (auto& mesh : m_meshes)
    {
        // Keep sure only this meshComponent has ownership on its meshs
        CORE_ASSERT(mesh.second.unique(), "Non-unique mesh about to be removed.");
        mesh.second.reset();
    }

    m_meshes.clear();
}

void Engine::FancyMeshDrawable::draw(const Core::Matrix4& viewMatrix,
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
        mesh.second->draw();
    }
}

void Engine::FancyMeshDrawable::draw(const Core::Matrix4& viewMatrix,
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
        mesh.second->draw();
    }
}

void Engine::FancyMeshDrawable::addMesh(Mesh* mesh)
{
    std::string name = mesh->getName();

    if (m_meshes.find(name) != m_meshes.end())
    {
        fprintf(stderr, "Warning, mesh %s has already been added.\n", name.c_str());
        return;
    }

    m_meshes.insert(MeshByName(name, std::shared_ptr<Mesh>(mesh)));
}

void Engine::FancyMeshDrawable::removeMesh(const std::string& name)
{
    auto mesh = m_meshes.find(name);
    if (mesh == m_meshes.end())
    {
        fprintf(stderr, "Warning, mesh %s does not exist.\n", name.c_str());
        return;
    }

    CORE_ASSERT(mesh->second.unique(), "Non-unique mesh about to be removed.");
    mesh->second.reset();
    m_meshes.erase(mesh);
}

void Engine::FancyMeshDrawable::removeMesh(Engine::Mesh* mesh)
{
    removeMesh(mesh->getName());
}

Engine::Mesh* Engine::FancyMeshDrawable::getMesh(const std::string &name) const
{
    auto mesh = m_meshes.find(name);
    Engine::Mesh* ret = nullptr;

    if (mesh != m_meshes.end())
    {
        ret = mesh->second.get();
    }

    return ret;
}

void Engine::FancyMeshDrawable::updateGLInternal()
{
	m_material->updateGL();
	for (auto& mesh : m_meshes)
	{
		mesh.second->updateGL();
	}

	m_isDirty = false;
}

Engine::Drawable* Engine::FancyMeshDrawable::cloneInternal()
{
	// FIXME(Charly): Did I forget to clone something ?
	FancyMeshDrawable* drawable = new FancyMeshDrawable(m_name);

	for (const auto& it : m_meshes)
	{
		Mesh* oldMesh = getMesh(it.first);
		Mesh* newMesh = new Mesh(oldMesh->getName());
		newMesh->loadGeometry(oldMesh->getMeshData(),
							  oldMesh->getTangents(),
							  oldMesh->getBitangents(),
							  oldMesh->getTexcoords());

		drawable->addMesh(newMesh);
	}

	// FancyMesh materials do not require particular attention,
	// just share the pointer.
	drawable->setMaterial(m_material);
	drawable->setVisible(m_visible);
	drawable->setComponent(m_component);

	return drawable;
}

//void Engine::FancyMeshDrawable::setSelected(bool selected)
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
