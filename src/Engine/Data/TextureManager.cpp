#include <Engine/Data/Texture.hpp>
#include <Engine/Data/TextureManager.hpp>

#include <Core/Utils/Log.hpp>

#include <stb/stb_image.h>

#include <memory>

namespace Ra {
namespace Engine {
namespace Data {

using namespace Core::Utils; // log

TextureManager::TextureManager() = default;

TextureManager::~TextureManager() {
    for ( auto& tex : m_textures ) {
        delete tex.second;
    }
    m_textures.clear();
    m_pendingTextures.clear();
    m_pendingData.clear();
}

TextureParameters& TextureManager::addTexture( const std::string& name,
                                               uint width,
                                               uint height,
                                               std::shared_ptr<void> data ) {
    TextureParameters texData;
    texData.name         = name;
    texData.image.width  = width;
    texData.image.height = height;
    texData.image.texels = data;

    m_pendingTextures[name] = texData;

    return m_pendingTextures[name];
}

void TextureManager::loadTextureImage( TextureParameters& texParameters ) {
    stbi_set_flip_vertically_on_load( true );
    int n;
    unsigned char* data = stbi_load( texParameters.name.c_str(),
                                     (int*)( &( texParameters.image.width ) ),
                                     (int*)( &( texParameters.image.height ) ),
                                     &n,
                                     0 );

    if ( !data ) {
        LOG( logERROR ) << "Something went wrong when loading image \"" << texParameters.name
                        << "\".";
        texParameters.image.width = texParameters.image.height = 0;
        return;
    }

    switch ( n ) {
    case 1: {
        texParameters.image.format         = GL_RED;
        texParameters.image.internalFormat = GL_R8;
    } break;

    case 2: {
        // suppose it is GL_LUMINANCE_ALPHA
        texParameters.image.format         = GL_RG;
        texParameters.image.internalFormat = GL_RG8;
    } break;

    case 3: {
        texParameters.image.format         = GL_RGB;
        texParameters.image.internalFormat = GL_RGB8;
    } break;

    case 4: {
        texParameters.image.format         = GL_RGBA;
        texParameters.image.internalFormat = GL_RGBA8;
    } break;
    default: {
        texParameters.image.format         = GL_RGBA;
        texParameters.image.internalFormat = GL_RGBA8;
    } break;
    }

    CORE_ASSERT( data, "Data is null" );
    texParameters.image.texels = std::shared_ptr<void>( data );
    texParameters.image.type   = GL_UNSIGNED_BYTE;
}

Texture* TextureManager::loadTexture( const TextureParameters& texParameters, bool linearize ) {
    TextureParameters texParams = texParameters;
    // No texels ? load image to texels
    if ( texParams.image.texels == nullptr ) { loadTextureImage( texParams ); }
    auto ret = new Texture( texParams );
    ret->initializeNow( linearize );
    return ret;
}

Texture* TextureManager::getOrLoadTexture( const TextureParameters& texParameters,
                                           bool linearize ) {
    {
        // Is texture in the manager ?
        auto it = m_textures.find( texParameters.name );
        if ( it != m_textures.end() ) { return it->second; }
    }
    {
        // Is texture pending but registered in the manager
        auto it = m_pendingTextures.find( texParameters.name );
        if ( it != m_pendingTextures.end() ) {
            auto pendingParams             = it->second;
            auto ret                       = loadTexture( pendingParams, linearize );
            m_textures[pendingParams.name] = ret;
            m_pendingTextures.erase( it );
            return ret;
        }
    }
    // Texture is not in the manager, add it
    auto ret = loadTexture( texParameters, linearize );

    m_textures[texParameters.name] = ret;
    return ret;
}

void TextureManager::deleteTexture( const std::string& filename ) {
    auto it = m_textures.find( filename );

    if ( it != m_textures.end() ) {
        delete it->second;
        m_textures.erase( it );
    }
}

void TextureManager::deleteTexture( Texture* texture ) {
    deleteTexture( texture->getName() );
}

void TextureManager::updateTextureContent( const std::string& texture,
                                           std::shared_ptr<void> content ) {
    CORE_ASSERT( m_textures.find( texture ) != m_textures.end(),
                 "Trying to update non existing texture" );
    m_pendingData[texture] = content;
}

void TextureManager::updatePendingTextures() {
    if ( m_pendingData.empty() ) { return; }

    for ( auto& data : m_pendingData ) {
        LOG( logINFO ) << "TextureManager::updateTextures \"" << data.first << "\".";
        m_textures[data.first]->updateData( data.second );
    }
    m_pendingData.clear();
}

} // namespace Data
} // namespace Engine
} // namespace Ra
