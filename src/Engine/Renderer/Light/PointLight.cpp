#include <Engine/Renderer/Light/PointLight.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{

    Engine::PointLight::PointLight( const std::string& name )
        : Light( Light::POINT, name )
        , m_position( 0, 0, 0 )
        , m_attenuation()
    {
    }

    Engine::PointLight::~PointLight()
    {
    }

    void Engine::PointLight::getRenderParameters( RenderParameters& params )
    {
        Light::getRenderParameters( params );

        params.addParameter( "light.point.position", m_position );
        params.addParameter( "light.point.attenuation.constant", m_attenuation.constant );
        params.addParameter( "light.point.attenuation.linear", m_attenuation.linear );
        params.addParameter( "light.point.attenuation.quadratic", m_attenuation.quadratic );
    }

}
