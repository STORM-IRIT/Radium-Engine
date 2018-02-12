#include <Engine/Renderer/Light/Light.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{
    namespace Engine
    {

        Light::Light( const LightType& type, const std::string& name )
            : Component( name )
            , m_color( 1.0, 1.0, 1.0, 1.0 )
            , m_type( type )
        {
        }

        Light::~Light()
        {
        }

        void Light::getRenderParameters( RenderParameters& params )
        {
            params.addParameter( "light.color", m_color );
            params.addParameter( "light.type", m_type );
        }

        void Light::initialize()
        {
            // Nothing to do.
        }

        std::string Light::getShaderInclude() const
        {
            return "";
        }

    }
}
