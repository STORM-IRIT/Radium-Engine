
namespace Ra {
namespace Engine {

inline void BlinnPhongMaterial::addTexture( const TextureSemantic& semantic, Texture* texture ) {
    m_textures[semantic] = texture;
}

inline TextureData& BlinnPhongMaterial::addTexture( const TextureSemantic& semantic,
                                                    const std::string& texture ) {
    CORE_ASSERT( !texture.empty(), "Invalid texture name" );

    TextureData data;
    data.name = texture;
    data.wrapS = GL_REPEAT;
    data.wrapT = GL_REPEAT;
    data.magFilter = GL_NEAREST;
    data.minFilter = GL_NEAREST;

    return addTexture( semantic, data );
}

inline TextureData& BlinnPhongMaterial::addTexture( const TextureSemantic& semantic,
                                                    const TextureData& texture ) {
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
