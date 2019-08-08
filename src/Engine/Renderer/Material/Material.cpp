#include <Engine/Renderer/Material/Material.hpp>

namespace Ra::Engine {

Material::Material( std::string instanceName,
                    std::string materialName,
                    MaterialAspect aspect ) :
    m_instanceName{std::move(instanceName)},
    m_aspect{aspect},
    m_materialName{std::move(materialName)} {}

bool Material::isTransparent() const {
    return m_aspect == MaterialAspect::MAT_TRANSPARENT;
}

} // namespace Ra::Engine
