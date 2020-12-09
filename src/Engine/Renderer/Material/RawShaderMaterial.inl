#include <Engine/Renderer/Material/RawShaderMaterial.hpp>

namespace Ra {
namespace Engine {

inline Material*
RawShaderMaterialConverter::operator()( const Ra::Core::Asset::MaterialData* toconvert ) {
    auto mat = static_cast<const Core::Asset::RawShaderMaterialData*>( toconvert );
    return new RawShaderMaterial( mat->getName(), mat->m_shaders, mat->m_paramProvider );
}

inline RenderParameters& RawShaderMaterial::getParameters() {
    return m_paramProvider->getParameters();
}

inline const RenderParameters& RawShaderMaterial::getParameters() const {
    return m_paramProvider->getParameters();
}

} // namespace Engine
} // namespace Ra
