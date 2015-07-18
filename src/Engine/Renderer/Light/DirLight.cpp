#include <Engine/Renderer/Light/DirLight.hpp>

#include <Engine/Renderer/Shader/ShaderProgram.hpp>

namespace Ra
{

Engine::DirectionalLight::DirectionalLight()
    : Light(Light::DIRECTIONAL)
    , m_direction(0, -1, 0)
{
}

Engine::DirectionalLight::~DirectionalLight()
{
}

void Engine::DirectionalLight::bind(ShaderProgram* shader)
{
    fprintf(stderr, "Binding DirLight\n");
    Light::bind(shader);

    shader->setUniform("light.directional.direction", m_direction);
}

}
