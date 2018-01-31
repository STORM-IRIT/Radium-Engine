#include <Engine/Renderer/Light/Light.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{

    Engine::Light::Light( const LightType& type, const std::string& name )
        : Component( name )
        , m_color( 1.0, 1.0, 1.0, 1.0 )
        , m_type( type )
    {
    }

    Engine::Light::~Light()
    {
    }

    void Engine::Light::getRenderParameters( RenderParameters& params )
    {
        params.addParameter( "light.color", m_color );
        params.addParameter( "light.type", m_type );
    }

    void Engine::Light::initialize()
    {
        // Nothing to do.
    }

}
