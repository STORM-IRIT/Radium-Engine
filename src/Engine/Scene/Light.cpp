#include <Engine/Scene/Light.hpp>

#include <Engine/Data/RenderParameters.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {
namespace Scene {
Light::Light( Scene::Entity* entity, const LightType& type, const std::string& name ) :
    Component( name, entity ), m_type( type ) {}

void Light::getRenderParameters( Data::RenderParameters& params ) const {
    params.addParameter( "light.color", m_color );
    params.addParameter( "light.type", m_type );
}

void Light::initialize() {
    // Nothing to do.
}

std::string Light::getShaderInclude() const {
    return {};
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
