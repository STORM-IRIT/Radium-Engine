#include <Engine/Renderer/RenderQueue/ShaderKey.hpp>

#include <Core/CoreMacros.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/RenderQueue/RenderParameters.hpp>

namespace Ra
{

Engine::ShaderKey::ShaderKey(ShaderProgram* shader)
    : m_shader(shader)
{
    CORE_ASSERT(shader, "Null shader passed to bindable shader.");
}

Engine::ShaderKey::~ShaderKey()
{
}

void Engine::ShaderKey::bind() const
{
    m_shader->bind();
}

void Engine::ShaderKey::bind(const RenderParameters &params) const
{
    m_shader->bind();
    params.bind(m_shader);
}

bool Engine::ShaderKey::operator<(const ShaderKey& other) const
{
    return m_shader->getId() < other.m_shader->getId();
}

} // namespace Ra
