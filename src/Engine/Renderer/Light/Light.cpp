#include <Engine/Renderer/Light/Light.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
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
            // By default this is anonymous, to have the EntityManger generate auto. name
            // for us. If you actually want to be able to name your entity, use the
            // Entity::rename() method. This is not the definitive behaviour.
            // FIXME (Hugo).
            m_entity = RadiumEngine::getInstance()->getEntityManager()->createEntity();
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
