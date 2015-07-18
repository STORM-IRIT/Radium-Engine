#include <Engine/Renderer/Light/SpotLight.hpp>

#include <Engine/Renderer/Shader/ShaderProgram.hpp>

namespace Ra
{
Engine::SpotLight::SpotLight()
    : Light(Light::SPOT)
    , m_position(0, 0, 0)
    , m_direction(0, -1, 0)
    , m_attenuation()
{
}

Engine::SpotLight::~SpotLight()
{
}

void Engine::SpotLight::bind(ShaderProgram *shader)
{
    Light::bind(shader);

    shader->setUniform("light.spot.position", m_position);
    shader->setUniform("light.spot.direction", m_direction);
    shader->setUniform("light.spot.innerAngle", m_innerAngle);
    shader->setUniform("light.spot.outerAngle", m_outerAngle);
    shader->setUniform("light.spot.attenuation.constant", m_attenuation.constant);
    shader->setUniform("light.spot.attenuation.linear", m_attenuation.linear);
    shader->setUniform("light.spot.attenuation.quadratic", m_attenuation.quadratic);
}

}
