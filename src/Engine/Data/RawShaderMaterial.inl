#pragma once
#include <Engine/Data/RawShaderMaterial.hpp>

namespace Ra {
namespace Engine {
namespace Data {
inline Material*
RawShaderMaterialConverter::operator()( const Ra::Core::Asset::MaterialData* toconvert ) {
    auto mat = static_cast<const Core::Asset::RawShaderMaterialData*>( toconvert );
    return new RawShaderMaterial( mat->getName(), mat->m_shaders, mat->m_paramProvider );
}

inline Rendering::RenderParameters& RawShaderMaterial::getParameters() {
    return m_paramProvider->getParameters();
}

inline const Rendering::RenderParameters& RawShaderMaterial::getParameters() const {
    return m_paramProvider->getParameters();
}

} // namespace Data
} // namespace Engine
} // namespace Ra
