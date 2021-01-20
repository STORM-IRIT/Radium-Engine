#include <Engine/Data/DirLight.hpp>

#include <Engine/Renderer/RenderParameters.hpp>

namespace Ra {
namespace Engine {
DirectionalLight::DirectionalLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::DIRECTIONAL, name ) {}

void DirectionalLight::getRenderParameters( RenderParameters& params ) const {
    Light::getRenderParameters( params );

    params.addParameter( "light.directional.direction", m_direction );
}

std::string DirectionalLight::getShaderInclude() const {
    return "Directional";
}

} // namespace Engine
} // namespace Ra
