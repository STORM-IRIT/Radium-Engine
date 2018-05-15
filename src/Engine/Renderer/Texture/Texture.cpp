#include <Engine/Renderer/Texture/Texture.hpp>

#include <globjects/Texture.h>

namespace Ra {
Engine::Texture::Texture( std::string name ) :
    /*m_textureId(0)
     , */
    m_name( name ),
    m_texture( nullptr ) {}

Engine::Texture::~Texture() {}

void Engine::Texture::Generate( uint w, GLenum format, void* data ) {
    m_target = GL_TEXTURE_1D;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->image1D( 0, internalFormat, w, 0, format, GL_UNSIGNED_BYTE, data );

    updateParameters();

    m_texture->generateMipmap();

    m_format = format;
    m_width = w;
}

void Engine::Texture::Generate( uint w, uint h, GLenum format, void* data ) {
    m_target = GL_TEXTURE_2D;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->image2D( 0, internalFormat, w, h, 0, format, dataType, data );

    updateParameters();

    m_texture->generateMipmap();

    m_format = format;
    m_width = w;
    m_height = h;
}

void Engine::Texture::Generate( uint w, uint h, uint d, GLenum format, void* data ) {
    m_target = GL_TEXTURE_3D;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->image3D( 0, internalFormat, w, h, d, 0, format, dataType, data );

    updateParameters();

    m_texture->generateMipmap();

    m_format = format;
    m_width = w;
    m_height = h;
    m_depth = d;
}

void Engine::Texture::GenerateCube( uint w, uint h, GLenum format, void** data ) {
    m_target = GL_TEXTURE_CUBE_MAP;
    if ( m_texture == nullptr )
    {
        m_texture = globjects::Texture::create( m_target );
    }

    m_texture->cubeMapImage( 0, internalFormat, w, h, 0, format, dataType, data );

    updateParameters();

    m_texture->generateMipmap();

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
    { CORE_ASSERT( 0, "Dafuck ?" ); }
    break;
    }
}

void Engine::Texture::updateParameters() {
    switch ( m_texture->target() ) {
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_3D:
        m_texture->setParameter( GL_TEXTURE_WRAP_R, wrapR );
        __attribute__ ((fallthrough)); // must be updated to [[fallthrough]] when compiler will be C++17
    case GL_TEXTURE_2D:
        m_texture->setParameter( GL_TEXTURE_WRAP_T, wrapT );
        __attribute__ ((fallthrough)); // must be updated to [[fallthrough]] when compiler will be C++17
    case GL_TEXTURE_1D:
        m_texture->setParameter( GL_TEXTURE_WRAP_S, wrapS );
        break;
    default:
        break;
    }
    m_texture->setParameter( GL_TEXTURE_MIN_FILTER, minFilter );
    m_texture->setParameter( GL_TEXTURE_MAG_FILTER, magFilter );
}
} // namespace Ra
