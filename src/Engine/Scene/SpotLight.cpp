#include <Engine/Renderer/RenderParameters.hpp>
#include <Engine/Scene/SpotLight.hpp>

namespace Ra {
namespace Engine {
namespace Scene {
SpotLight::SpotLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::SPOT, name ) {}

void SpotLight::getRenderParameters( Renderer::RenderParameters& params ) const {
    Light::getRenderParameters( params );

    params.addParameter( "light.spot.position", m_position );
    params.addParameter( "light.spot.direction", m_direction );
    params.addParameter( "light.spot.innerAngle", m_innerAngle );
    params.addParameter( "light.spot.outerAngle", m_outerAngle );
    params.addParameter( "light.spot.attenuation.constant", m_attenuation.constant );
    params.addParameter( "light.spot.attenuation.linear", m_attenuation.linear );
    params.addParameter( "light.spot.attenuation.quadratic", m_attenuation.quadratic );
}

std::string SpotLight::getShaderInclude() const {
    return "Spot";
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
