#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <cstdio>

#include <FreeImage.h>

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

            FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
            FIBITMAP* dib = nullptr; // TODO This name is nonsense, change it

            // Find format from file signature
            fif = FreeImage_GetFileType( filename.c_str(), 0 );

            if ( FIF_UNKNOWN == fif )
            {
                // Find format from file extension
                fif = FreeImage_GetFIFFromFilename( filename.c_str() );
            }

            if ( FIF_UNKNOWN == fif )
            {
                // Still unknown
                std::string error = "Cannot determine " + filename + " image format.";
                LOG( logERROR ) << error;
                return nullptr;
            }

            if ( FreeImage_FIFSupportsReading( fif ) )
            {
                dib = FreeImage_Load( fif, filename.c_str() );
            }

            std::string error = "Something went wrong while trying to load " + filename + ".";

            if ( nullptr == dib )
            {
                LOG( logERROR ) << error;
                return nullptr;
            }

            ret = new Texture( filename, GL_TEXTURE_2D );
            unsigned char* data = FreeImage_GetBits( dib );

            int bpp = FreeImage_GetBPP( dib );
            int format = 0, internal = 0;
            switch ( bpp )
            {
                case 8:
                {
                    // FIXME(Charly): Debug this
                    format = GL_RED;
                    internal = GL_RED;
                } break;

                case 24:
                {
                    format = GL_BGR;
                    internal = GL_RGB;
                } break;

                case 32:
                {
                    format = GL_BGRA;
                    internal = GL_RGBA;
                } break;

                default:
                {
                    LOG( logERROR ) << "Unknow bytes per pixel format : " << bpp;
                }
            }

            int w = FreeImage_GetWidth( dib );
            int h = FreeImage_GetHeight( dib );

            if ( m_verbose )
            {
                LOG( logINFO ) << "Image stats (" << filename << ") :\n"
                               << "\tBPP    : 0x" << std::hex << bpp << std::dec << std::endl
                               << "\tFormat : 0x" << std::hex << format << std::dec << std::endl
                               << "\tSize   : " << w << ", " << h;
            }


            CORE_ASSERT( data, "Data is null" );

            ret->initGL( internal, w, h, format, GL_UNSIGNED_BYTE, data );
            ret->genMipmap( GL_LINEAR, GL_LINEAR );

            m_textures.insert( TexturePair( filename, ret ) );

            FreeImage_Unload( dib );

            return ret;
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
                    ret = new Texture( filename, GL_TEXTURE_2D );
                    ret->initGL( GL_RGBA, pending->second.width, pending->second.height,
                                 GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pending->second.data );

                    ret->genMipmap( GL_LINEAR, GL_LINEAR );
                    ret->setClamp( GL_CLAMP, GL_CLAMP );

                    m_pendingTextures.erase( filename );
                    m_textures[filename] = ret;
                }
                else
                {
                    ret = addTexture( filename );
                    ret->setClamp( GL_CLAMP, GL_CLAMP );
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
