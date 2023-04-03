#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Scene/SpotLight.hpp>

namespace Ra {
namespace Engine {
namespace Scene {
SpotLight::SpotLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::SPOT, name ) {
    getRenderParameters().addParameter( "light.spot.position", m_position );
    getRenderParameters().addParameter( "light.spot.direction", m_direction );
    getRenderParameters().addParameter( "light.spot.innerAngle", m_innerAngle );
    getRenderParameters().addParameter( "light.spot.outerAngle", m_outerAngle );
    getRenderParameters().addParameter( "light.spot.attenuation.constant", m_attenuation.constant );
    getRenderParameters().addParameter( "light.spot.attenuation.linear", m_attenuation.linear );
    getRenderParameters().addParameter( "light.spot.attenuation.quadratic",
                                        m_attenuation.quadratic );
}

std::string SpotLight::getShaderInclude() const {
    return "Spot";
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
