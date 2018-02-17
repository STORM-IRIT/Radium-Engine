#include <Engine/Renderer/Light/SpotLight.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{
    Engine::SpotLight::SpotLight( const std::string& name )
        : Light( Light::SPOT, name )
        , m_position( 0, 0, 0 )
        , m_direction( 0, -1, 0 )
        , m_attenuation()
    {
    }

    Engine::SpotLight::~SpotLight()
    {
    }

    void Engine::SpotLight::getRenderParameters( RenderParameters& params )
    {
        Light::getRenderParameters( params );

        params.addParameter( "light.spot.position", m_position );
        params.addParameter( "light.spot.direction", m_direction );
        params.addParameter( "light.spot.innerAngle", m_innerAngle );
        params.addParameter( "light.spot.outerAngle", m_outerAngle );
        params.addParameter( "light.spot.attenuation.constant", m_attenuation.constant );
        params.addParameter( "light.spot.attenuation.linear", m_attenuation.linear );
        params.addParameter( "light.spot.attenuation.quadratic", m_attenuation.quadratic );
    }

}
