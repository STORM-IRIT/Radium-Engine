#pragma once
#include <Engine/Data/VolumetricMaterial.hpp>

namespace Ra {
namespace Engine {
namespace Data {
// Add a texture as material parameter from an already existing Radium Texture
inline void VolumetricMaterial::setTexture( Texture* texture ) {
    m_texture = texture;
}

inline Texture* VolumetricMaterial::getTexture() const {
    return m_texture;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
