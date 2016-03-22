#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include <Engine/Renderer/Texture/stb_image.h>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{
    namespace Engine
    {
        TextureManager::TextureManager()
            : m_verbose( false )
        {
        }

        TextureManager::~TextureManager()
        {
            for ( auto& tex : m_textures )
            {
                delete tex.second;
            }
            m_textures.clear();
        }

        void TextureManager::addTexture( const std::string& name, int width, int height, void* data )
        {
            TextureData texData;
            texData.name = name;
            texData.width = width;
            texData.height = height;
            texData.data = data;

            m_pendingTextures[name] = texData;
        }

        Texture* TextureManager::addTexture( const std::string& filename )
        {
            Texture* ret = nullptr;

            int w, h, n;
            stbi_set_flip_vertically_on_load(true);

            unsigned char* data = stbi_load(filename.c_str(), &w, &h, &n, 0);

            if (!data)
            {
                LOG(logERROR) << "Something went wrong when loading image \"" << filename << "\".";
                return nullptr;
            }

            GLuint format;
            switch(n)
            {
                case 1: format = GL_RED;    break;
                case 2: format = GL_RG;     break;
                case 3: format = GL_RGB;    break;
                case 4: format = GL_RGBA;   break;
            }

            if ( m_verbose )
            {
                LOG( logINFO ) << "Image stats (" << filename << ") :\n"
                               << "\tPixels : " << n << std::endl
                               << "\tFormat : 0x" << std::hex << format << std::dec << std::endl
                               << "\tSize   : " << w << ", " << h;
            }

            CORE_ASSERT( data, "Data is null" );

            ret = new Texture(filename, GL_TEXTURE_2D);
            ret->initGL( format, w, h, format, GL_UNSIGNED_BYTE, data );
            ret->genMipmap( GL_LINEAR, GL_LINEAR );

            m_textures.insert( TexturePair( filename, ret ) );

            stbi_image_free(data);

            return ret;
        }

        Texture* TextureManager::getOrLoadTexture(const TextureData &data)
        {
            m_pendingTextures[data.name] = data;
            return getOrLoadTexture(data.name);
        }

        Texture* TextureManager::getOrLoadTexture( const std::string& filename )
        {
            Texture* ret = nullptr;
            auto it = m_textures.find( filename );

            if ( it != m_textures.end() )
            {
                ret = it->second;
            }
            else
            {
                auto pending = m_pendingTextures.find( filename );
                if ( pending != m_pendingTextures.end() )
                {
                    auto data = pending->second;
                    if (data.data != nullptr)
                    {
                        ret = new Texture( filename, GL_TEXTURE_2D );
                        ret->initGL( GL_RGBA, pending->second.width, pending->second.height,
                                     GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pending->second.data );
                    }
                    else
                    {
                        ret = addTexture(data.name);
                    }

                    ret->genMipmap(data.minMipmap, data.magMipmap);
                    ret->setClamp(data.sWrap, data.tWrap);

                    m_pendingTextures.erase( filename );
                    m_textures[filename] = ret;
                }
                else
                {
                    ret = addTexture( filename );
                    ret->setClamp( GL_REPEAT, GL_REPEAT );
                }
            }

            return ret;
        }

        void TextureManager::deleteTexture( const std::string& filename )
        {
            auto it = m_textures.find( filename );

            if ( it != m_textures.end() )
            {
                delete it->second;
                m_textures.erase( it );
            }
        }

        void TextureManager::deleteTexture( Texture* texture )
        {
            deleteTexture( texture->getName() );
        }

        void TextureManager::updateTexture( const std::string& texture, void* data )
        {
            CORE_ASSERT( m_textures.find( texture ) != m_textures.end(),
                         "Trying to update non existing texture" );
            m_pendingData[texture] = data;
        }

        void TextureManager::updateTextures()
        {
            if ( m_pendingData.empty() )
            {
                return;
            }

            for ( auto& data : m_pendingData )
            {
                m_textures[data.first]->updateData( data.second );
            }
            m_pendingData.clear();
        }

        RA_SINGLETON_IMPLEMENTATION(TextureManager);
    }
} // namespace Ra
