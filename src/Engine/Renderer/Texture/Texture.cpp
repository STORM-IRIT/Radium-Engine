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
    m_isLinear{false},
    m_texels {nullptr} {}

Engine::Texture::~Texture() {}

void Engine::Texture::Generate(uint w, GLenum format, void *data, bool linearize, bool mipmaped)
{
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

void Engine::Texture::Generate(uint w, uint h, GLenum format, void *data, bool linearize, bool mipmaped) {
    m_target = GL_TEXTURE_2D;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    updateParameters();
    m_isMipMaped = mipmaped;

    m_format = format;
    m_width = w;
    m_height = h;

    // Load texels
    if (linearize) {
        int numcomp = 0;
        bool hasAlpha = false;
        switch (m_format) {
            // RED texture store a gray scale color. Verify if we need to convert
        case GL_RED :
            numcomp = 1;
            break;
        case GL_RGB :
            numcomp = 3;
            break;
        case GL_RGBA :
            numcomp = 4;
            hasAlpha = true;
            break;
        default :
            LOG(logERROR) << "Textures with format " << m_format << " can't be linearized." << m_name;
            return;
        }
        // This will do the conversion then upload texels on GPU and generates mipmap if needed.
        sRGBToLinearRGB(reinterpret_cast<uint8_t *>(data), numcomp, hasAlpha);
    } else {
        // only upload to the GPU and generate mipmap if needed
        m_texture->image2D( 0, internalFormat, w, h, 0, format, dataType, data );
        if (m_isMipMaped)
        {
            m_texture->generateMipmap();
        }
    }

}

void Engine::Texture::Generate(uint w, uint h, uint d, GLenum format, void *data, bool linearize, bool mipmaped) {
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

void Engine::Texture::GenerateCube(uint w, uint h, GLenum format, void **data, bool linearize, bool mipmaped) {
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

void Engine::Texture::linearize(Scalar gamma) {
    if (! m_isLinear)
    {
        // Only RGB and RGBA texture contains color information
        // (others are not really colors and must be managed explicitely by the user)
        int numcomp = 0;
        bool hasAlpha = false;
        switch (m_format) {
            // RED texture store a gray scale color. Verify if we need to convert
        case GL_RED :
            numcomp = 1;
            break;
        case GL_RGB :
            numcomp = 3;
            break;
        case GL_RGBA :
            numcomp = 4;
            hasAlpha = true;
            break;
        default :
            LOG(logERROR) << "Textures with format " << m_format << " can't be linearized." << m_name;
            return;
        }
        std::vector<uint8_t> texPixels;
        texPixels.resize(m_width*m_height*m_depth*numcomp);
        m_texture->getImage(0, m_format, dataType, texPixels.data());
        sRGBToLinearRGB(texPixels.data(), numcomp, hasAlpha, gamma);
    }
}

void Engine::Texture::sRGBToLinearRGB(uint8_t *texels, int numCommponent, bool hasAlphaChannel, Scalar gamma)
{
    if (! m_isLinear) {
        m_isLinear = true;
        //auto linearize = [gamma](float in)-> float {
        auto linearize = [gamma](uint8_t in)-> unsigned char {
            // Constants are described at https://en.wikipedia.org/wiki/SRGB
            float c = float(in)/255;
            if (c < 0.04045) {
                c = c/ 12.92;
            } else
            {
                c = std::pow(((c + 0.055) / (1 + 0.055)), float(gamma));
            }
            return uint8_t(c*255);
        };
        int numvalues = hasAlphaChannel ? numCommponent - 1 : numCommponent;
#pragma omp parallel for
        for (int i=0; i<m_width*m_height*m_depth; ++i) {
            // Convert each R or RGB value while keeping alpha unchanged
            for (int p=i*numCommponent; p < i*numCommponent + numvalues; ++p) {
                texels[p] = linearize(texels[p]);
            }
        }
        updateData(texels);
        if (m_isMipMaped) {
            m_texture->generateMipmap();
        }
    }
}
} // namespace Ra
