#include "DummyComponent.hpp"

#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

namespace DummyPlugin
{
    DummyComponent::DummyComponent( const std::string& name )
        : Ra::Engine::Component( name )
    {
    }

    DummyComponent::~DummyComponent()
    {
    }

    void DummyComponent::initialize()
    {
    }

} // namespace FancyMeshPlugin
