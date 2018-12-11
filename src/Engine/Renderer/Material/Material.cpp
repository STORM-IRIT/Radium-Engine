#include <Engine/Renderer/Material/Material.hpp>

namespace Ra {
namespace Engine {

Material::Material( const std::string& name, MaterialAspect aspect ) :
    m_name { name },
    m_aspect { aspect } {}


bool Material::isTransparent() const {
    return m_aspect == MaterialAspect::MAT_TRANSPARENT;
}

const std::string Material::getShaderInclude() const {
    return {};
}

} // namespace Engine
} // namespace Ra
