#include <Engine/Scene/PointLight.hpp>

#include <Engine/Renderer/RenderParameters.hpp>

namespace Ra {
namespace Engine {
namespace Scene {
PointLight::PointLight( Entity* entity, const std::string& name ) :
    Light( entity, Light::POINT, name ) {}

void PointLight::getRenderParameters( Renderer::RenderParameters& params ) const {
    Light::getRenderParameters( params );

    params.addParameter( "light.point.position", m_position );
    params.addParameter( "light.point.attenuation.constant", m_attenuation.constant );
    params.addParameter( "light.point.attenuation.linear", m_attenuation.linear );
    params.addParameter( "light.point.attenuation.quadratic", m_attenuation.quadratic );
}

std::string PointLight::getShaderInclude() const {
    return "Point";
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
