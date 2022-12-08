#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Scene/SpotLight.hpp>

namespace Ra {
namespace Engine {
namespace Scene {
SpotLight::SpotLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::SPOT, name ) {
    m_params.addParameter( "light.spot.position", m_position );
    m_params.addParameter( "light.spot.direction", m_direction );
    m_params.addParameter( "light.spot.innerAngle", m_innerAngle );
    m_params.addParameter( "light.spot.outerAngle", m_outerAngle );
    m_params.addParameter( "light.spot.attenuation.constant", m_attenuation.constant );
    m_params.addParameter( "light.spot.attenuation.linear", m_attenuation.linear );
    m_params.addParameter( "light.spot.attenuation.quadratic", m_attenuation.quadratic );
}

std::string SpotLight::getShaderInclude() const {
    return "Spot";
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
