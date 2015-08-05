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
	RenderTechnique* technique = new RenderTechnique;
	technique->material = new Material("Default");
	technique->shaderConfig = ShaderConfiguration("Default", "../Shaders");

	addMeshRenderObject(mesh, name, technique);
}

void Engine::FancyMeshComponent::addMeshRenderObject(const Core::TriangleMesh& mesh, const std::string& name, RenderTechnique* technique)
{
    RenderObject* renderObject = new RenderObject(name);
    renderObject->setComponent(this);
    renderObject->setVisible(true);

    renderObject->setRenderTechnique(technique);

	Mesh* displayMesh = new Mesh(name);
	std::vector<uint> indices;
	indices.reserve(mesh.m_triangles.size() * 3);
	for (const auto& i : mesh.m_triangles)
	{
		indices.push_back(i.x());
		indices.push_back(i.y());
		indices.push_back(i.z());
	}

	displayMesh->loadGeometry(mesh.m_vertices, indices);
	displayMesh->addData(Mesh::VERTEX_NORMAL, mesh.m_normals);

    renderObject->setMesh(displayMesh);
    m_renderObject = m_renderObjectManager->addRenderObject(renderObject);
}


void Engine::FancyMeshComponent::handleMeshLoading(const FancyComponentData& data)
{
	CORE_ASSERT(data.meshes.size() == 1, "One mesh per component / object.");
	// FIXME(Charly): Change data meshes array to just one mesh

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

		mesh->loadGeometry(meshData.positions, meshData.indices);
		
		mesh->addData(Mesh::VERTEX_NORMAL, meshData.normals);
		mesh->addData(Mesh::VERTEX_TANGENT, meshData.tangents);
		mesh->addData(Mesh::VERTEX_BITANGENT, meshData.bitangents);
		mesh->addData(Mesh::VERTEX_TEXCOORD, meshData.texcoords);

        renderObject->setMesh(mesh);
	}

    renderObject->setRenderTechnique(data.renderTechnique);

    m_renderObject = m_renderObjectManager->addRenderObject(renderObject);
}

}
