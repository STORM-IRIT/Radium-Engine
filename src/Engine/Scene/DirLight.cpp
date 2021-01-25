#include <Engine/Scene/DirLight.hpp>

#include <Engine/Renderer/RenderParameters.hpp>

namespace Ra {
namespace Engine {
namespace Scene {
DirectionalLight::DirectionalLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::DIRECTIONAL, name ) {}

void DirectionalLight::getRenderParameters( Renderer::RenderParameters& params ) const {
    Light::getRenderParameters( params );

    params.addParameter( "light.directional.direction", m_direction );
}

std::string DirectionalLight::getShaderInclude() const {
    return "Directional";
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
