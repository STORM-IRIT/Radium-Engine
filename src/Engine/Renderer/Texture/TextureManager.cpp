#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <Core/Log/Log.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Ra {
namespace Engine {
TextureManager::TextureManager() : m_verbose( false ) {}

TextureManager::~TextureManager() {
    for ( auto& tex : m_textures )
    {
        delete tex.second;
    }
    m_textures.clear();
}

TextureData& TextureManager::addTexture( const std::string& name, int width, int height,
                                         void* data ) {
    TextureData texData;
    texData.name = name;
    texData.width = width;
    texData.height = height;
    texData.data = data;

    m_pendingTextures[name] = texData;

    return m_pendingTextures[name];
}

TextureData TextureManager::loadTexture( const std::string& filename ){
    TextureData texData;
    texData.name = filename;

    stbi_set_flip_vertically_on_load( true );

    int  n;
    unsigned char* data = stbi_load( filename.c_str(), &(texData.width), &(texData.height), &n, 0 );

    if ( !data )
    {
        LOG( logERROR ) << "Something went wrong when loading image \"" << filename << "\".";
        texData.width = texData.height = -1;
        return texData;
    }

    switch ( n )
    {
    case 1:
    {
        texData.format = GL_RED;
        texData.internalFormat = GL_R8;
    }
        break;

    case 2:
    {
        texData.format = GL_RG;
        texData.internalFormat = GL_RG8;
    }
        break;

    case 3:
    {
        texData.format = GL_RGB;
        texData.internalFormat = GL_RGB8;
    }
        break;

    case 4:
    {
        texData.format = GL_RGBA;
        texData.internalFormat = GL_RGBA8;
    }
        break;
    default:
    {
        texData.format = GL_RGBA;
        texData.internalFormat = GL_RGBA8;
    }
        break;
    }

    if ( m_verbose )
    {
        LOG( logINFO ) << "Image stats (" << filename << ") :\n"
                       << "\tPixels : " << n << std::endl
                       << "\tFormat : " << texData.format <<  std::endl
                       << "\tSize   : " << texData.width << ", " << texData.height;
    }

    CORE_ASSERT( data, "Data is null" );
    texData.data = data;
    texData.type = GL_UNSIGNED_BYTE;
    return texData;

}

Texture* TextureManager::getOrLoadTexture( const TextureData& data ) {
    m_pendingTextures[data.name] = data;
    return getOrLoadTexture( data.name );
}

Texture* TextureManager::getOrLoadTexture( const std::string& filename ) {
    Texture* ret = nullptr;
    auto it = m_textures.find( filename );

    if ( it != m_textures.end() )
    {
        ret = it->second;
    } else {
        auto pending = m_pendingTextures.find( filename );
        if ( pending != m_pendingTextures.end() )
        {
            auto data = pending->second;

            bool freedata = false;
            if (data.data == nullptr) {
                auto stbidata = loadTexture(data.name);
                data.width = stbidata.width;
                data.height = stbidata.height;
                data.data = stbidata.data;
                data.type = stbidata.type;
                data.format = stbidata.format;
                data.internalFormat = stbidata.internalFormat;
                freedata = true;
            }

            ret = new Texture( filename );
            ret->internalFormat = data.internalFormat;
            ret->dataType = data.type;
            ret->minFilter = data.minFilter;
            ret->magFilter = data.magFilter;
            ret->wrapS = data.wrapS;
            ret->wrapT = data.wrapT;
            ret->Generate( data.width, data.height, data.format, data.data );

            if (freedata)
                stbi_image_free( data.data );

            m_pendingTextures.erase( filename );
            m_textures[filename] = ret;

        } else {
            auto data = loadTexture(filename);
            ret = new Texture( filename );
            ret->internalFormat = data.internalFormat;
            ret->dataType = data.type;
            ret->minFilter = data.minFilter;
            ret->magFilter = data.magFilter;
            ret->wrapS = data.wrapS;
            ret->wrapT = data.wrapT;
            ret->Generate( data.width, data.height, data.format, data.data );
            stbi_image_free( data.data );
            m_textures[filename] = ret;
        }
    }

    return ret;
}

void TextureManager::deleteTexture( const std::string& filename ) {
    auto it = m_textures.find( filename );

    if ( it != m_textures.end() )
    {
        delete it->second;
        m_textures.erase( it );
    }
}

void TextureManager::deleteTexture( Texture* texture ) {
    deleteTexture( texture->getName() );
}

void TextureManager::updateTexture( const std::string& texture, void* data ) {
    CORE_ASSERT( m_textures.find( texture ) != m_textures.end(),
                 "Trying to update non existing texture" );
    m_pendingData[texture] = data;
}

void TextureManager::updateTextures() {
    if ( m_pendingData.empty() )
    {
        return;
    }

    for ( auto& data : m_pendingData )
    {
        LOG( logINFO ) << "TextureManager::updateTextures \"" << data.first << "\".";
        m_textures[data.first]->updateData( data.second );
    }
    m_pendingData.clear();
}

RA_SINGLETON_IMPLEMENTATION( TextureManager );
} // namespace Engine
} // namespace Ra
