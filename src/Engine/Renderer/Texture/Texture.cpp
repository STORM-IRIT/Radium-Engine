#include <Engine/Renderer/Texture/Texture.hpp>

#include <cstdio>

namespace Ra
{

    Engine::Texture::Texture( std::string name, uint target, TextureType type, uint zoffset )
        : m_textureId( 0 )
        , m_name( name )
        , m_target( target )
        , m_type( type )
        , m_zoffset( zoffset )
        , m_pixels( nullptr )
        , m_bytesPerPixel( 0 )
        , m_width( 1 )
        , m_height( 1 )
        , m_depth( 1 )
    {
    }

    Engine::Texture::~Texture()
    {
        deleteGL();
        delete[] m_pixels;
    }

    void Engine::Texture::setBPP( int bpp )
    {
        switch ( bpp )
        {
            case GL_LUMINANCE:
            // FIXME: Check alpha
            case GL_LUMINANCE_ALPHA:
            case GL_RED:
            {
                m_bytesPerPixel = 1;
            }
            break;

            case GL_DEPTH_COMPONENT24:
            case GL_RGB:
            case GL_SRGB:
            case GL_SRGB8:
            {
                m_bytesPerPixel = 3;
            }
            break;

            case GL_DEPTH24_STENCIL8:
            case GL_RGBA:
            case GL_BGRA:
            case GL_R32F:
            {
                m_bytesPerPixel = 4;
            }
            break;

            case GL_RG32F:
            {
                m_bytesPerPixel = 8;
            }
            break;

            case GL_RGB32F:
            {
                m_bytesPerPixel = 12;
            }
            break;

            case GL_RGBA32F:
            {
                m_bytesPerPixel = 16;
            }
            break;

            default:
            {
                LOG( logERROR ) << "Problem with texture color mode (0x" << std::hex << bpp << std::dec << ")"
                                << "- (Texture " << m_name << ").\n";
                m_bytesPerPixel = 0;
            }
        }
    }

    void Engine::Texture::initGL( uint bpp, uint w, uint format, uint type, void* data )
    {
        GL_ASSERT( glGenTextures( 1, &m_textureId ) );
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );
        GL_ASSERT( glTexImage1D( m_target, 0, bpp, w, 0, format, type, data ) );

        GL_ASSERT( glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );

        setBPP( bpp );
        m_format = format;

        m_dataType = type;

        m_width = w;

        if ( data && m_bytesPerPixel )
        {
            m_pixels = new unsigned char[m_width * m_bytesPerPixel];
            std::memcpy( m_pixels, data, m_width * m_bytesPerPixel );
        }
    }

    void Engine::Texture::initGL( uint internal, uint w, uint h, uint format, uint type, void* data )
    {
        GL_ASSERT( glGenTextures( 1, &m_textureId ) );
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        GL_ASSERT( glTexImage2D( m_target, 0, internal, w, h, 0, format, type, data ) );

        setBPP( internal );
        m_format = format;

        m_dataType = type;

        m_width  = w;
        m_height = h;

        if ( data && m_bytesPerPixel )
        {
            m_pixels = new unsigned char[m_width * m_height * m_bytesPerPixel];
            CORE_ASSERT( memcpy( m_pixels, data, m_width * m_height * m_bytesPerPixel ), "THIS IS FACT UP" );
        }
    }

    void Engine::Texture::initGL( uint bpp, uint w, uint h, uint d, uint format, uint type, void* data )
    {
        GL_ASSERT( glGenTextures( 1, &m_textureId ) );
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );
        GL_ASSERT( glTexImage3D( m_target, 0, bpp, w, h, d, 0, format, type, data ) );

        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );

        setBPP( bpp );
        m_format = format;

        m_dataType = type;

        m_width  = w;
        m_height = h;
        m_depth  = d;

        if ( data && m_bytesPerPixel )
        {
            m_pixels = new unsigned char[m_width * m_height * m_depth * m_bytesPerPixel];
            memcpy( m_pixels, data, m_width * m_height * m_depth * m_bytesPerPixel );
        }
    }

    void Engine::Texture::initCubeGL( uint bpp, uint w, uint h, uint format, uint type, void** data )
    {
        GL_ASSERT( glGenTextures( 1, &m_textureId ) );
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        // FIXME Type was forced to GL_Scalar, check calls for this method.
        GL_ASSERT( glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, bpp, w, h, 0, format, type, data[0] ) );
        GL_ASSERT( glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, bpp, w, h, 0, format, type, data[1] ) );
        GL_ASSERT( glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, bpp, w, h, 0, format, type, data[2] ) );
        GL_ASSERT( glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, bpp, w, h, 0, format, type, data[3] ) );
        GL_ASSERT( glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, bpp, w, h, 0, format, type, data[4] ) );
        GL_ASSERT( glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, bpp, w, h, 0, format, type, data[5] ) );

        GL_ASSERT( glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ) );

        GL_ASSERT( glGenerateMipmap( m_target ) );
        /*
          GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
          GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
          GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        */
    }

    void Engine::Texture::genMipmap( GLenum minFilter, GLenum magFilter )
    {
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        GL_ASSERT( glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, minFilter ) );
        GL_ASSERT( glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, magFilter ) );

        GL_ASSERT( glGenerateMipmap( m_target ) );
    }

    void Engine::Texture::setFilter( uint minFilter, uint magFilter )
    {
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        GL_ASSERT( glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, minFilter ) );
        GL_ASSERT( glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, magFilter ) );
    }

    void Engine::Texture::setClamp( GLenum wrapS )
    {
        assert( m_type == TEXTURE_1D && "setClamp(s) cannot be called for this texture type." );

        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        GL_ASSERT( glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrapS ) );
    }

    void Engine::Texture::setClamp( GLenum wrapS, GLenum wrapT )
    {
        CORE_ASSERT( ( m_type == TEXTURE_2D ) || ( m_type == TEXTURE_CUBE ),
                     "setClamp(s, t) cannot be called for this texture type." );

        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        if ( m_type == TEXTURE_2D )
        {
            GL_ASSERT( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS ) );
            GL_ASSERT( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT ) );
        }
        else
        {
            GL_ASSERT( glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS ) );
            GL_ASSERT( glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT ) );
        }
    }

    void Engine::Texture::setClamp( GLenum wrapS, GLenum wrapT, GLenum wrapR )
    {
        CORE_ASSERT( m_type == TEXTURE_3D, "setClamp(s, t, r) cannot be called for this texture type." );

        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapS ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapT ) );
        GL_ASSERT( glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapR ) );
    }

    void Engine::Texture::bind( int unit )
    {
        GLenum texUnit = GL_TEXTURE0 + unit;
        GL_ASSERT( glActiveTexture( texUnit ) );
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );
    }

    void Engine::Texture::deleteGL()
    {
        GL_ASSERT( glDeleteTextures( 1, &m_textureId ) );
    }

    uint Engine::Texture::getId() const
    {
        return m_textureId;
    }

    uint Engine::Texture::getTarget() const
    {
        return m_target;
    }

    Engine::Texture::TextureType Engine::Texture::getType() const
    {
        return m_type;
    }

    std::string Engine::Texture::getName() const
    {
        return m_name;
    }

    uint Engine::Texture::getZOffset() const
    {
        return m_zoffset;
    }

    Core::Color Engine::Texture::getTexel( uint u, uint v )
    {
        Scalar* pixels = new Scalar[m_width * m_height * 4];

        GL_ASSERT( glBindTexture( GL_TEXTURE_2D, m_textureId ) );
        GL_ASSERT( glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels ) );

        uint idx = ( v * m_width + u ) * 4;
        Core::Color color( pixels[idx + 0], pixels[idx + 1], pixels[idx + 2], 1.0 );

        delete[] pixels;

        return color;
    }

    void Engine::Texture::updateData( void* data )
    {
        GL_ASSERT( glBindTexture( m_target, m_textureId ) );

        switch ( m_type )
        {
            case TEXTURE_1D:
            case TEXTURE_3D:
            case TEXTURE_CUBE:
            {
                CORE_ASSERT( 0, "Not implemented." );
            } break;

            case TEXTURE_2D:
            {
                GL_ASSERT( glTexImage2D( m_target, 0, m_bytesPerPixel, m_width, m_height,
                                         0, m_format, m_dataType, data ) );
            } break;

            default:
            {
                CORE_ASSERT( 0, "Dafuck ?" );
            } break;
        }

        if ( data )
        {
            if ( !m_pixels )
            {
                m_pixels = new unsigned char[m_width * m_height * m_depth * m_bytesPerPixel];
            }
            memcpy( m_pixels, data, m_width * m_height * m_depth * m_bytesPerPixel );
        }
    }

} // namespace Ra
