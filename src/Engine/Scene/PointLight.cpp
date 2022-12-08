#include <Engine/Scene/PointLight.hpp>

#include <Engine/Data/RenderParameters.hpp>

namespace Ra {
namespace Engine {
namespace Scene {
PointLight::PointLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::POINT, name ) {
    m_params.addParameter( "light.point.position", m_position );
    m_params.addParameter( "light.point.attenuation.constant", m_attenuation.constant );
    m_params.addParameter( "light.point.attenuation.linear", m_attenuation.linear );
    m_params.addParameter( "light.point.attenuation.quadratic", m_attenuation.quadratic );
}

std::string PointLight::getShaderInclude() const {
    return "Point";
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
