#include <Engine/Renderer/FancyMeshPlugin/FancyMeshComponent.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Renderer/Drawable/DrawableManager.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshDrawable.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Material/Material.hpp>

namespace Ra
{

Engine::FancyMeshComponent::FancyMeshComponent()
	: Component()
{
}

Engine::FancyMeshComponent::~FancyMeshComponent()
{
	// TODO(Charly): Should we ask the drawable manager to delete our drawable ?
	m_drawableManager->removeDrawable(m_drawable);
}

void Engine::FancyMeshComponent::initialize()
{
}

void Engine::FancyMeshComponent::update(Scalar dt)
{
}

void Engine::FancyMeshComponent::handleMeshLoading(const FancyComponentData& data)
{
	FancyMeshDrawable* drawable = new FancyMeshDrawable(data.name);
	drawable->setComponent(this);
	drawable->setVisible(true);

    fprintf(stderr, "About to create drawable\n");

	for (uint i = 0; i < data.meshes.size(); ++i)
	{
		FancyMeshData meshData = data.meshes[i];

		std::string meshName;
		Core::StringUtils::stringPrintf(meshName, "%s_mesh_%u", 
                                        data.name.c_str(), i);

		Mesh* mesh = new Mesh(meshName);
		mesh->loadGeometry(meshData.mesh, meshData.tangents, 
						   meshData.bitangents, meshData.texcoords);
		drawable->addMesh(mesh);
	}

	drawable->setMaterial(data.material);

	m_drawable = m_drawableManager->addDrawable(drawable);

    fprintf(stderr, "Done creating drawable, index : %u\n", m_drawable.getValue());
}

}
