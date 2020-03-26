#include <Engine/Renderer/Light/Light.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra {
namespace Engine {

Light::Light( Entity* entity, const LightType& type, const std::string& name ) :
    Component( name, entity ), m_type( type ) {}

void Light::getRenderParameters( RenderParameters& params ) const {
    params.addParameter( "light.color", m_color );
    params.addParameter( "light.type", m_type );
}

void Light::initialize() {
    // Nothing to do.
}

std::string Light::getShaderInclude() const {
    return {};
}

} // namespace Engine
} // namespace Ra
