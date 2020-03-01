#include <Engine/Renderer/Material/VolumetricMaterial.hpp>

namespace Ra {
namespace Engine {

// Add a texture as material parameter from an already existing Radium Texture
inline void VolumetricMaterial::setTexture( Texture* texture ) {
    m_texture = texture;
}

inline Texture* VolumetricMaterial::getTexture() const {
    return m_texture;
}
} // namespace Engine
} // namespace Ra
