#include <Engine/Data/Texture.hpp>
#include <Engine/Data/TextureManager.hpp>

#include <Core/Utils/Log.hpp>

#include <globjects/AbstractUniform.h>
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
}

TextureParameters& TextureManager::addTexture2( const std::string& name,
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

TextureManager::TextureHandle TextureManager::addTexture( const TextureParameters& parameters ) {
    // find first free slot in m_newTextures
    auto texture = std::make_unique<Texture>( parameters );
    TextureHandle handle;
    auto it = std::find_if( m_newTextures.begin(), m_newTextures.end(), []( const auto& texture ) {
        return !texture;
    } );
    if ( it != m_newTextures.end() ) {
        it->swap( texture );
        handle.setValue( std::distance( m_newTextures.begin(), it ) );
    }
    else {
        m_newTextures.push_back( std::move( texture ) );
        handle.setValue( m_newTextures.size() - 1 );
    }
    m_newTextures[handle.getValue()]->initialize();
    return handle;
}

ImageParameters TextureManager::loadTextureImage( const std::string& filename, bool linearize ) {
    stbi_set_flip_vertically_on_load( true );
    int n;
    ImageParameters image;
    unsigned char* data = stbi_load(
        filename.c_str(), (int*)( &( image.width ) ), (int*)( &( image.height ) ), &n, 0 );

    if ( !data ) {
        LOG( logERROR ) << "Something went wrong when loading image \"" << filename << "\".";
        image.width = image.height = 0;
        return image;
    }

    switch ( n ) {
    case 1: {
        image.format         = GL_RED;
        image.internalFormat = GL_R8;
    } break;

    case 2: {
        // suppose it is GL_LUMINANCE_ALPHA
        image.format         = GL_RG;
        image.internalFormat = GL_RG8;
    } break;

    case 3: {
        image.format         = GL_RGB;
        image.internalFormat = GL_RGB8;
    } break;

    case 4: {
        image.format         = GL_RGBA;
        image.internalFormat = GL_RGBA8;
    } break;
    default: {
        image.format         = GL_RGBA;
        image.internalFormat = GL_RGBA8;
    } break;
    }

    CORE_ASSERT( data, "Data is null" );
    image.texels = std::shared_ptr<void>( data );
    image.type   = GL_UNSIGNED_BYTE;
    if ( linearize ) Texture::linearize( image );
    return image;
}

Texture* TextureManager::loadTexture2( const TextureParameters& texParameters, bool linearize ) {
    TextureParameters texParams = texParameters;
    // No texels ? load image to texels
    if ( texParams.image.texels == nullptr ) {
        auto image      = loadTextureImage( texParams.name );
        texParams.image = image;
    }
    auto ret = new Texture( texParams );
    ret->initializeNow( linearize );
    return ret;
}

Texture* TextureManager::getOrLoadTexture2( const TextureParameters& texParameters,
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
            auto ret                       = loadTexture2( pendingParams, linearize );
            m_textures[pendingParams.name] = ret;
            m_pendingTextures.erase( it );
            return ret;
        }
    }
    // Texture is not in the manager, add it
    auto ret = loadTexture2( texParameters, linearize );

    m_textures[texParameters.name] = ret;
    return ret;
}

Texture* TextureManager::getTexture( const TextureHandle& handle ) {
    return handle.isValid() ? m_newTextures[handle.getValue()].get() : nullptr;
}

TextureManager::TextureHandle TextureManager::getTextureHandle( const std::string& name ) {
    auto it = std::find_if( m_newTextures.begin(),
                            m_newTextures.end(),
                            [name]( const auto& texture ) { return texture->getName() == name; } );

    return it != m_newTextures.end() ? TextureHandle { std::distance( m_newTextures.begin(), it ) }
                                     : TextureHandle::Invalid();
};

void TextureManager::deleteTexture2( const std::string& filename ) {
    auto it = m_textures.find( filename );

    if ( it != m_textures.end() ) {
        delete it->second;
        m_textures.erase( it );
    }
}

void TextureManager::deleteTexture2( Texture* texture ) {
    deleteTexture2( texture->getName() );
}

void TextureManager::deleteTexture( const TextureHandle& handle ) {
    if ( handle.isValid() ) m_newTextures[handle.getValue()].reset( nullptr );
}

void TextureManager::updateTextureContent( const std::string& texture,
                                           std::shared_ptr<void> content ) {
    CORE_ASSERT( m_textures.find( texture ) != m_textures.end(),
                 "Trying to update non existing texture" );
    m_textures[texture]->updateData( content );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
