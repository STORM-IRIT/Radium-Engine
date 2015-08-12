#include <Engine/Renderer/OpenGL/FBO.hpp>

#include <cstdio>

#include <Core/String/StringUtils.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{

    Engine::FBO::FBO( Components components, uint width, uint height )
        : m_components( components )
        , m_width( width ? width : 1 )
        , m_height( height ? height : 1 )
        , m_isBound( false )
    {
        GL_ASSERT( glGenFramebuffers( 1, &m_fboID ) );
    }

    Engine::FBO::~FBO()
    {
        GL_ASSERT( glDeleteFramebuffers( 1, &m_fboID ) );
        m_textures.clear();
    }

    void Engine::FBO::bind()
    {
        GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, m_fboID ) );
        m_isBound = true;
    }

    void Engine::FBO::useAsTarget()
    {
        bind();
        GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );
    }

    void Engine::FBO::useAsTarget( uint width, uint height )
    {
        bind();
        GL_ASSERT( glViewport( 0, 0, width, height ) );
    }

    void Engine::FBO::unbind( bool complete )
    {
        m_isBound = false;
        if ( complete )
        {
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );
        }
    }

    void Engine::FBO::attachTexture( uint attachment, Engine::Texture* texture, uint level )
    {
        assert( m_isBound && "FBO must be bound to attach a texture." );

        switch ( texture->getType() )
        {
            case Engine::Texture::TEXTURE_1D:
            {
                GL_ASSERT( glFramebufferTexture1D( GL_FRAMEBUFFER, attachment, texture->getTarget(),
                                                   texture->getId(), level ) );
            }
            break;

            case Engine::Texture::TEXTURE_2D:
            case Engine::Texture::TEXTURE_CUBE:
            {
                GL_ASSERT( glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, texture->getTarget(),
                                                   texture->getId(), level ) );
            }
            break;

            case Engine::Texture::TEXTURE_3D:
            {
                GL_ASSERT( glFramebufferTexture3D( GL_FRAMEBUFFER, attachment, texture->getTarget(),
                                                   texture->getId(), level, texture->getZOffset() ) );
            }
            break;
        }

        m_textures[attachment] = texture;
    }

    void Engine::FBO::detachTexture( uint attachment )
    {
        assert( m_isBound && "FBO must be bound to detach a texture." );
        assert( m_textures.find( attachment ) != m_textures.end() &&
                "Trying to detach a non attached texture." );

        Engine::Texture* texture = m_textures[attachment];

        switch ( texture->getType() )
        {
            case Engine::Texture::TEXTURE_1D:
            {
                GL_ASSERT( glFramebufferTexture1D( GL_FRAMEBUFFER, attachment, 0, 0, 0 ) );
            }
            break;

            case Engine::Texture::TEXTURE_2D:
            case Engine::Texture::TEXTURE_CUBE:
            {
                GL_ASSERT( glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, 0, 0, 0 ) );
            }
            break;

            case Engine::Texture::TEXTURE_3D:
            {
                GL_ASSERT( glFramebufferTexture3D( GL_FRAMEBUFFER, attachment, 0, 0, 0, 0 ) );
            }
            break;
        }

        m_textures.erase( attachment );
    }

    void Engine::FBO::check() const
    {
        // TODO: More verbose check
        GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

        if ( GL_FRAMEBUFFER_COMPLETE != fboStatus )
        {
            std::string err;
            switch ( fboStatus )
            {
                case GL_FRAMEBUFFER_UNDEFINED:
                {
                    err = "GL_FRAMEBUFFER_UNDEFINED";
                }
                break;
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                {
                    err = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                }
                break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                {
                    err = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                }
                break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                {
                    err = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
                }
                break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                {
                    err = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
                }
                break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                {
                    err = "GL_FRAMEBUFFER_UNSUPPORTED";
                }
                break;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                {
                    err = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
                }
                break;
                case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                {
                    err = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
                }
                break;
                default:
                {
                    Core::StringUtils::stringPrintf( err, "Unknown %08x", fboStatus );
                }
            }

            LOG( logERROR ) << "FBO Error : " << err;
        }

        assert( fboStatus == GL_FRAMEBUFFER_COMPLETE && "Something went wrong with the Framebuffer.\n" );
    }

    void Engine::FBO::setSize( uint width, uint height )
    {
        m_width = width;
        m_height = height;
    }

    void Engine::FBO::clear( Components components )
    {
        assert( m_isBound && "FBO must be bound before calling clear()." );

        Components nc = Components( m_components & components );

        GLbitfield mask = 0;

        if ( nc & COLOR )
        {
            mask |= GL_COLOR_BUFFER_BIT;
        }

        if ( nc & DEPTH )
        {
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        if ( nc & STENCIL )
        {
            mask |= GL_STENCIL_BUFFER_BIT;
        }

        GL_ASSERT( glClear( mask ) );
    }

} // namespace Ra
