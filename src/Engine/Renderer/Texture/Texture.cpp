#include <Core/Utils/Log.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

#include "Texture.hpp"
#include <globjects/Texture.h>

#include <cmath>

namespace Ra {

using namespace Core::Utils; // log

Engine::Texture::Texture( const TextureParameters& texParameters ) :
    m_textureParameters{texParameters},
    m_texture{nullptr},
    m_isMipMaped{false},
    m_isLinear{false} {}

Engine::Texture::~Texture() = default;

void Engine::Texture::initializeGL( bool linearize ) {
    if ( ( m_textureParameters.target != GL_TEXTURE_1D ) &&
         ( m_textureParameters.target != GL_TEXTURE_2D ) &&
         ( m_textureParameters.target != GL_TEXTURE_RECTANGLE ) &&
         ( m_textureParameters.target != GL_TEXTURE_3D ) &&
         ( m_textureParameters.target != GL_TEXTURE_CUBE_MAP ) )
    {
        LOG( logERROR ) << "Texture of type " << m_textureParameters.target
                        << " must be generated explicitely!";
        return;
    }
    // Transform texels if needed
    if ( linearize )
    {
        uint numcomp  = 0;
        bool hasAlpha = false;
        switch ( m_textureParameters.format )
        {
            // RED and RG texture store a gray scale color. Verify if we need to convert
        case GL_RED:
            numcomp = 1;
            break;
        case GL_RG:
            numcomp  = 1;
            hasAlpha = true;
            break;
        case GL_RGB:
            numcomp = 3;
            break;
        case GL_RGBA:
            numcomp  = 4;
            hasAlpha = true;
            break;
        default:
            LOG( logERROR ) << "Textures with format " << m_textureParameters.format
                            << " can't be linearized." << m_textureParameters.name;
            return;
        }
        if ( m_textureParameters.target == GL_TEXTURE_CUBE_MAP )
        { linearizeCubeMap( numcomp, hasAlpha ); }
        else
        {
            // This will only do do the RGB space conversion
            sRGBToLinearRGB(
                reinterpret_cast<uint8_t*>( m_textureParameters.texels ), numcomp, hasAlpha );
        }
    }
    // Generate OpenGL texture
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_textureParameters.target );
        GL_CHECK_ERROR;
    }
    // Update the sampler parameters
    m_isMipMaped = !( m_textureParameters.minFilter == GL_NEAREST ||
                      m_textureParameters.minFilter == GL_LINEAR );
    updateParameters();
    // upload texture to the GPU
    updateData( m_textureParameters.texels );
    // Generate mipmap if needed.
    if ( m_isMipMaped ) { m_texture->generateMipmap(); }
}

void Engine::Texture::bind( int unit ) {
    if ( unit >= 0 ) { m_texture->bindActive( uint( unit ) ); }
    else
    { m_texture->bind(); }
}

void Engine::Texture::bindImageTexture( int unit,
                                        const GLint level,
                                        const GLboolean layered,
                                        const GLint layer,
                                        const GLenum access ) {
    m_texture->bindImageTexture(
        uint( unit ), level, layered, layer, access, m_textureParameters.internalFormat );
}

void Engine::Texture::updateData( void* data ) {
    switch ( m_texture->target() )
    {
    case GL_TEXTURE_1D: {
        m_texture->image1D( 0,
                            m_textureParameters.internalFormat,
                            GLsizei( m_textureParameters.width ),
                            0,
                            m_textureParameters.format,
                            m_textureParameters.type,
                            data );
        GL_CHECK_ERROR
    }
    break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE: {
        m_texture->image2D( 0,
                            m_textureParameters.internalFormat,
                            GLsizei( m_textureParameters.width ),
                            GLsizei( m_textureParameters.height ),
                            0,
                            m_textureParameters.format,
                            m_textureParameters.type,
                            data );
        GL_CHECK_ERROR
    }
    break;
    case GL_TEXTURE_3D: {
        m_texture->image3D( 0,
                            m_textureParameters.internalFormat,
                            GLsizei( m_textureParameters.width ),
                            GLsizei( m_textureParameters.height ),
                            GLsizei( m_textureParameters.depth ),
                            0,
                            m_textureParameters.format,
                            m_textureParameters.type,
                            data );
        GL_CHECK_ERROR
    }
    break;
    case GL_TEXTURE_CUBE_MAP: {
        // Load the 6 faces of the cubemap
        void** texels = (void**)data;
        m_texture->bind();
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                          0,
                          m_textureParameters.internalFormat,
                          GLsizei( m_textureParameters.width ),
                          GLsizei( m_textureParameters.height ),
                          0,
                          m_textureParameters.format,
                          m_textureParameters.type,
                          texels[0] );
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                          0,
                          m_textureParameters.internalFormat,
                          GLsizei( m_textureParameters.width ),
                          GLsizei( m_textureParameters.height ),
                          0,
                          m_textureParameters.format,
                          m_textureParameters.type,
                          texels[1] );

        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                          0,
                          m_textureParameters.internalFormat,

                          GLsizei( m_textureParameters.width ),
                          GLsizei( m_textureParameters.height ),
                          0,
                          m_textureParameters.format,
                          m_textureParameters.type,
                          texels[2] );
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                          0,
                          m_textureParameters.internalFormat,
                          GLsizei( m_textureParameters.width ),
                          GLsizei( m_textureParameters.height ),
                          0,
                          m_textureParameters.format,
                          m_textureParameters.type,
                          texels[3] );

        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                          0,
                          m_textureParameters.internalFormat,
                          GLsizei( m_textureParameters.width ),
                          GLsizei( m_textureParameters.height ),
                          0,
                          m_textureParameters.format,
                          m_textureParameters.type,
                          texels[4] );
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                          0,
                          m_textureParameters.internalFormat,
                          GLsizei( m_textureParameters.width ),
                          GLsizei( m_textureParameters.height ),
                          0,
                          m_textureParameters.format,
                          m_textureParameters.type,
                          texels[5] );

        m_texture->unbind();
        GL_CHECK_ERROR
    }
    break;
    default: {
        CORE_ASSERT( 0, "Unsupported texture type ?" );
    }
    break;
    }
    GL_CHECK_ERROR;
}

// let the compiler warn about case fallthrough
void Engine::Texture::updateParameters() {
    switch ( m_texture->target() )
    {
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_3D:
        m_texture->setParameter( GL_TEXTURE_WRAP_R, m_textureParameters.wrapP );
        GL_CHECK_ERROR;
        [[fallthrough]];
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE:
        m_texture->setParameter( GL_TEXTURE_WRAP_T, m_textureParameters.wrapT );
        GL_CHECK_ERROR;
        [[fallthrough]];
    case GL_TEXTURE_1D:
        m_texture->setParameter( GL_TEXTURE_WRAP_S, m_textureParameters.wrapS );
        GL_CHECK_ERROR;
        break;
    default:
        break;
    }
    m_texture->setParameter( GL_TEXTURE_MIN_FILTER, m_textureParameters.minFilter );
    GL_CHECK_ERROR;
    m_texture->setParameter( GL_TEXTURE_MAG_FILTER, m_textureParameters.magFilter );
    GL_CHECK_ERROR;
}

void Engine::Texture::linearize() {
    if ( m_texture != nullptr )
    {
        LOG( logERROR ) << "Only non OpenGL initialized texture can be linearized.";
        return;
    }
    // Only RGB and RGBA texture contains color information
    // (others are not really colors and must be managed explicitely by the user)
    uint numcomp  = 0;
    bool hasAlpha = false;
    switch ( m_textureParameters.format )
    {
        // RED texture store a gray scale color. Verify if we need to convert
    case GL_RED:
        numcomp = 1;
        break;
    case GL_RGB:
        numcomp = 3;
        break;
    case GL_RGBA:
        numcomp  = 4;
        hasAlpha = true;
        break;
    default:
        LOG( logERROR ) << "Textures with format " << m_textureParameters.format
                        << " can't be linearized." << m_textureParameters.name;
        return;
    }
    sRGBToLinearRGB( reinterpret_cast<uint8_t*>( m_textureParameters.texels ), numcomp, hasAlpha );
}

void Engine::Texture::sRGBToLinearRGB( uint8_t* texels, uint numCommponent, bool hasAlphaChannel ) {
    if ( !m_isLinear )
    {
        m_isLinear = true;
        // auto linearize = [gamma](float in)-> float {
        auto linearize = []( uint8_t in ) -> uint8_t {
            // Constants are described at https://en.wikipedia.org/wiki/SRGB
            float c = float( in ) / 255;
            if ( c < 0.04045 ) { c = c / 12.92f; }
            else
            { c = std::pow( ( ( c + 0.055f ) / ( 1.055f ) ), 2.4f ); }
            return uint8_t( c * 255 );
        };
        uint numvalues = hasAlphaChannel ? numCommponent - 1 : numCommponent;
#pragma omp parallel for
        for ( int i = 0; i < int( m_textureParameters.width * m_textureParameters.height *
                                  m_textureParameters.depth );
              ++i )
        {
            // Convert each R or RGB value while keeping alpha unchanged
            for ( uint p = i * numCommponent; p < i * numCommponent + numvalues; ++p )
            {
                texels[p] = linearize( texels[p] );
            }
        }
    }
}

void Engine::Texture::resize( size_t w, size_t h, size_t d, void* pix ) {
    m_textureParameters.width  = w;
    m_textureParameters.height = h;
    m_textureParameters.depth  = d;
    m_textureParameters.texels = pix;
    if ( m_texture == nullptr ) { initializeGL( false ); }
    else
    { updateData( m_textureParameters.texels ); }
    if ( m_isMipMaped ) { m_texture->generateMipmap(); }
}

void Engine::Texture::linearizeCubeMap( uint numCommponent, bool hasAlphaChannel ) {
    if ( m_textureParameters.type == gl::GLenum::GL_UNSIGNED_BYTE )
    {
        /// Only unsigned byte texture could be linearized. Considering other formats where already
        /// linear
        for ( int i = 0; i < 6; ++i )
        {
            sRGBToLinearRGB(
                reinterpret_cast<uint8_t*>( ( (void**)m_textureParameters.texels )[i] ),
                numCommponent,
                hasAlphaChannel );
        }
    }
}

} // namespace Ra
