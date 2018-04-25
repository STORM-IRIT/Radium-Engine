#include <Engine/Renderer/Material/Material.hpp>

namespace Ra {
namespace Engine {

Material::Material( const std::string& name, MaterialAspect aspect ) :
    m_name( name ),
    m_isDirty( true ),
    m_aspect( MaterialAspect::MAT_OPAQUE ) {}

Material::~Material() {}

bool Material::isTransparent() const {
    return m_aspect == MaterialAspect::MAT_TRANSPARENT;
}

const std::string Material::getShaderInclude() const {
    return std::string( "" );
}

} // namespace Engine
} // namespace Ra
