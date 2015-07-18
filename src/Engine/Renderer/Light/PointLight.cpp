#include <Engine/Renderer/Light/PointLight.hpp>

#include <Engine/Renderer/Shader/ShaderProgram.hpp>

namespace Ra
{

Engine::PointLight::PointLight()
    : Light(Light::POINT)
    , m_position(0, 0, 0)
    , m_attenuation()
{
}

Engine::PointLight::~PointLight()
{
}

void Engine::PointLight::bind(ShaderProgram* shader)
{
    Light::bind(shader);

    shader->setUniform("light.point.position", m_position);
    shader->setUniform("light.point.attenuation.constant", m_attenuation.constant);
    shader->setUniform("light.point.attenuation.linear", m_attenuation.linear);
    shader->setUniform("light.point.attenuation.quadratic", m_attenuation.quadratic);
}

}
