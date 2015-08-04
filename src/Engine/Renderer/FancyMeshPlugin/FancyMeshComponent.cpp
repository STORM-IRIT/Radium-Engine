#include <Engine/Renderer/FancyMeshPlugin/FancyMeshComponent.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshRenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

namespace Ra
{

Engine::FancyMeshComponent::FancyMeshComponent(const std::string& name)
    : Component(name)
{
}

Engine::FancyMeshComponent::~FancyMeshComponent()
{
    // TODO(Charly): Should we ask the RO manager to delete our render object ?
    m_renderObjectManager->removeRenderObject(m_renderObject);
}

void Engine::FancyMeshComponent::initialize()
{
}

void Engine::FancyMeshComponent::addMeshRenderObject(const Core::TriangleMesh& mesh, const std::string& name)
{
    RenderObject* renderObject = new RenderObject(name);
    renderObject->setComponent(this);
    renderObject->setVisible(true);

    Mesh* displayMesh = new Mesh(name);
    displayMesh->loadGeometry(mesh);
    renderObject->setMesh(displayMesh);

    RenderTechnique* technique = new RenderTechnique;
    technique->material = new Material("Default");
    technique->shaderConfig = ShaderConfiguration("Default", "../Shaders");
    renderObject->setRenderTechnique(technique);
    m_renderObject = m_renderObjectManager->addRenderObject(renderObject);
}

void Engine::FancyMeshComponent::addMeshRenderObject(const Core::TriangleMesh& mesh, const std::string& name, RenderTechnique* technique)
{
    RenderObject* renderObject = new RenderObject(name);
    renderObject->setComponent(this);
    renderObject->setVisible(true);

    renderObject->setRenderTechnique(technique);

    Mesh* displayMesh = new Mesh(name);
    displayMesh->loadGeometry(mesh);
    renderObject->setMesh(displayMesh);
    m_renderObject = m_renderObjectManager->addRenderObject(renderObject);
}


void Engine::FancyMeshComponent::handleMeshLoading(const FancyComponentData& data)
{
    RenderObject* renderObject = new RenderObject(data.name);
    renderObject->setComponent(this);
    renderObject->setVisible(true);

	for (uint i = 0; i < data.meshes.size(); ++i)
	{
		FancyMeshData meshData = data.meshes[i];

		std::stringstream ss;
		ss << data.name << "_mesh_" << i;
		std::string meshName = ss.str();

		Mesh* mesh = new Mesh(meshName);
		mesh->loadGeometry(meshData.mesh, meshData.tangents,
						   meshData.bitangents, meshData.texcoords);
        renderObject->setMesh(mesh);
	}

    renderObject->setRenderTechnique(data.renderTechnique);

    m_renderObject = m_renderObjectManager->addRenderObject(renderObject);
}


}
