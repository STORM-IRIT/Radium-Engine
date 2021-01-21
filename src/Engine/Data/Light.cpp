#include <Engine/Data/Light.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderParameters.hpp>

namespace Ra {
namespace Engine {
namespace Data {
Light::Light( Scene::Entity* entity, const LightType& type, const std::string& name ) :
    Component( name, entity ), m_type( type ) {}

void Light::getRenderParameters( Renderer::RenderParameters& params ) const {
    params.addParameter( "light.color", m_color );
    params.addParameter( "light.type", m_type );
}

void Light::initialize() {
    // Nothing to do.
}

std::string Light::getShaderInclude() const {
    return {};
}

} // namespace Data
} // namespace Engine
} // namespace Ra
