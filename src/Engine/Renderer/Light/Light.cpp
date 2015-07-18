#include <Engine/Renderer/Light/Light.hpp>

#include <Engine/Renderer/Shader/ShaderProgram.hpp>

namespace Ra
{

Engine::Light::Light(const LightType& type)
    : m_type(type)
    , m_color(1.0, 1.0, 1.0, 1.0)
{
}

Engine::Light::~Light()
{
}

void Engine::Light::bind(ShaderProgram* shader)
{
    shader->setUniform("light.color", m_color);
    shader->setUniform("light.type", m_type);
}

}
