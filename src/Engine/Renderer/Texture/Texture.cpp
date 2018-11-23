#include <Engine/Renderer/Texture/Texture.hpp>
#include <Core/Log/Log.hpp>

#include <globjects/Texture.h>
#include "Texture.hpp"

#include <cmath>

namespace Ra {
Engine::Texture::Texture( const std::string &name ) :
    m_name {name},
    m_width {1},
    m_height {1},
    m_depth {1},
    m_texture {nullptr},
    m_isMipMaped {false},
    m_isLinear{false} {}

Engine::Texture::~Texture() {}

void Engine::Texture::Generate( uint w, GLenum format, void* data, bool mipmaped ) {
    m_target = GL_TEXTURE_1D;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->image1D( 0, internalFormat, w, 0, format, dataType, data );

    updateParameters();

    if (mipmaped)
    {
        m_isMipMaped = true;
        m_texture->generateMipmap();
    }

    m_format = format;
    m_width = w;
}

void Engine::Texture::Generate( uint w, uint h, GLenum format, void* data, bool mipmaped ) {
    m_target = GL_TEXTURE_2D;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->image2D( 0, internalFormat, w, h, 0, format, dataType, data );

    updateParameters();

    if (mipmaped)
    {
        m_isMipMaped = true;
        m_texture->generateMipmap();
    }

    m_format = format;
    m_width = w;
    m_height = h;
}

void Engine::Texture::Generate( uint w, uint h, uint d, GLenum format, void* data, bool mipmaped ) {
    m_target = GL_TEXTURE_3D;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->image3D( 0, internalFormat, w, h, d, 0, format, dataType, data );

    updateParameters();

    if (mipmaped)
    {
        m_isMipMaped = true;
        m_texture->generateMipmap();
    }

    m_format = format;
    m_width = w;
    m_height = h;
    m_depth = d;
}

void Engine::Texture::GenerateCube( uint w, uint h, GLenum format, void** data, bool mipmaped ) {
    m_target = GL_TEXTURE_CUBE_MAP;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->cubeMapImage( 0, internalFormat, w, h, 0, format, dataType, data );

    updateParameters();

    if (mipmaped)
    {
        m_isMipMaped = true;
        m_texture->generateMipmap();
    }

    m_format = format;
    m_width = w;
    m_height = h;
}

void Engine::Texture::bind( int unit ) {
    if ( unit >= 0 )
    {
        m_texture->bindActive( unit );
    } else
    { m_texture->bind(); }
}

void Engine::Texture::updateData( void* data ) {
    switch ( m_texture->target() )
    {
    case GL_TEXTURE_1D:
    { m_texture->image1D( 0, internalFormat, m_width, 0, m_format, dataType, data ); }
    break;
    case GL_TEXTURE_2D:
    { m_texture->image2D( 0, internalFormat, m_width, m_height, 0, m_format, dataType, data ); }
    break;
    case GL_TEXTURE_3D:
    {
        m_texture->image3D( 0, internalFormat, m_width, m_height, m_depth, 0, m_format, dataType,
                            data );
    }
    break;
    case GL_TEXTURE_CUBE_MAP:
    {
        m_texture->cubeMapImage( 0, internalFormat, m_width, m_height, 0, m_format, dataType,
                                 data );
    }
    break;
    default:
    { CORE_ASSERT( 0, "Unsupported texture type ?" ); }
    break;
    }
}

// let the compiler warn about case fallthrough
void Engine::Texture::updateParameters() {
    switch ( m_texture->target() )
    {
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_3D:
        m_texture->setParameter( GL_TEXTURE_WRAP_R, wrapR );
        [[fallthrough]];
    case GL_TEXTURE_2D:
        m_texture->setParameter( GL_TEXTURE_WRAP_T, wrapT );
        [[fallthrough]];
    case GL_TEXTURE_1D:
        m_texture->setParameter( GL_TEXTURE_WRAP_S, wrapS );
        break;
    default:
        break;
    }
    m_texture->setParameter( GL_TEXTURE_MIN_FILTER, minFilter );
    m_texture->setParameter( GL_TEXTURE_MAG_FILTER, magFilter );
}

void Engine::Texture::sRGBToLinearRGB(Scalar gamma)
{
    if (! m_isLinear) {
        auto linearize = [gamma](float in)-> float {
            // Constants are described at https://en.wikipedia.org/wiki/SRGB
            if (in < 0.04045) {
                return in/ 12.92;
            } else
            {
                return std::pow(((in + 0.055) / (1 + 0.055)), float(gamma));
            }
        };
        // Only RGB and RGBA texture contains color information
        // (others are not really colors and must be managed explicitely by the user)
        int numcomp = 0;
        switch (internalFormat) {
        case GL_RGB :
        case GL_RGB8 :
        case GL_RGB16 :
        case GL_RGB16F :
        case GL_RGB32F :
            numcomp = 3;
            internalFormat = GL_RGB;
            m_format = GL_RGB;
            break;
        case GL_RGBA :
        case GL_RGBA8 :
        case GL_RGBA16 :
        case GL_RGBA16F :
        case GL_RGBA32F :
            numcomp = 4;
            internalFormat = GL_RGBA;
            m_format = GL_RGBA;
            break;
        default :
            LOG(logERROR) << "Textures with internal format " << internalFormat << " can't be linearized.";
            return;
        }
        dataType = GL_FLOAT;
        std::vector<float> texPixels;
        // we will have always an RGBA pixelbubberf
        texPixels.resize(m_width*m_height*m_depth*numcomp);
        m_texture->getImage(0, internalFormat, dataType, texPixels.data());
        // Convert each RGB value while keeping alpha unchanged
        for (int i=0; i<m_width*m_height*m_depth; ++i) {
            texPixels[i*numcomp    ] = linearize(texPixels[i*numcomp]);
            texPixels[i*numcomp + 1] = linearize(texPixels[i*numcomp + 1]);
            texPixels[i*numcomp + 2] = linearize(texPixels[i*numcomp + 2]);
        }
        updateData(texPixels.data());
        if (m_isMipMaped) {
            m_texture->generateMipmap();
        }
    }

}
} // namespace Ra
