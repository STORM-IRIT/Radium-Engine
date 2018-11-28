#include <Engine/Renderer/Light/SpotLight.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra {
namespace Engine {
SpotLight::SpotLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::SPOT, name ),
    m_position( 0, 0, 0 ),
    m_direction( 0, -1, 0 ),
    m_attenuation() {}

void SpotLight::getRenderParameters( RenderParameters& params ) const {
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

} // namespace Engine
} // namespace Ra
