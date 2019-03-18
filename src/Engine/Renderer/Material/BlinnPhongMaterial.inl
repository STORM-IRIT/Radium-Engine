#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>

namespace Ra {
namespace Engine {

// Add a texture as material parameter from an already existing Radium Texture
inline void BlinnPhongMaterial::addTexture( const TextureSemantic& semantic, Texture* texture ) {
    m_textures[semantic] = texture;
    // remove pendingTexture with same semantic, since the latter would
    // overwrite the former when updateGL will be called.
    m_pendingTextures.erase( semantic );
}

// Add a texture as material parameter with texture parameter set by default for this material
inline TextureParameters& BlinnPhongMaterial::addTexture( const TextureSemantic& semantic,
                                                          const std::string& texture ) {
    CORE_ASSERT( !texture.empty(), "Invalid texture name" );

    TextureParameters data;
    data.name = texture;
    data.wrapS = GL_REPEAT;
    data.wrapT = GL_REPEAT;
    if ( semantic != TextureSemantic::TEX_NORMAL )
    {
        data.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    }
    return addTexture( semantic, data );
}

// Add a texture as material parameter with texture parameter set by the caller
// The textures will be finalized (i.e loaded from a file if needed and transformed to OpenGL
// texture) only when needed by the updateGL method.
inline TextureParameters& BlinnPhongMaterial::addTexture( const TextureSemantic& semantic,
                                                          const TextureParameters& texture ) {
    m_pendingTextures[semantic] = texture;
    m_isDirty = true;

    return m_pendingTextures[semantic];
}

inline Texture* BlinnPhongMaterial::getTexture( const TextureSemantic& semantic ) const {
    Texture* tex = nullptr;

    auto it = m_textures.find( semantic );
    if ( it != m_textures.end() )
    {
        tex = it->second;
    }

    return tex;
}
} // namespace Engine
} // namespace Ra
