#include <Engine/Renderer/Light/DirLight.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra::Engine {
DirectionalLight::DirectionalLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::DIRECTIONAL, name ) {}

void DirectionalLight::getRenderParameters( RenderParameters& params ) const {
    Light::getRenderParameters( params );

    params.addParameter( "light.directional.direction", m_direction );
}

std::string DirectionalLight::getShaderInclude() const {
    return "Directional";
}

} // namespace Ra::Engine
