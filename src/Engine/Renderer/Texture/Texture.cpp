#include <Engine/Renderer/Texture/Texture.hpp>

#include <cstdio>

namespace Ra
{
    Engine::Texture::Texture(std::string name)
        : m_textureId(0)
        , m_name(name)
    {
    }

    Engine::Texture::~Texture()
    {
        deleteGL();
    }

    void Engine::Texture::Generate(uint w, uint format, void* data)
    {
        if (!glIsTexture(m_textureId)) GL_ASSERT(glGenTextures(1, &m_textureId));
        GL_ASSERT(glBindTexture(target, m_textureId));
        GL_ASSERT(glTexImage1D(target, 0, internalFormat, w, 0, format, dataType, data));

        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT ) );
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter));

        GL_ASSERT(glGenerateMipmap(target));

        m_format = format;
        m_width = w;
    }

    void Engine::Texture::Generate(uint w, uint h, uint format, void* data)
    {
        CORE_ASSERT(target == GL_TEXTURE_2D, "Wrong texture target");

        if (!glIsTexture(m_textureId)) GL_ASSERT(glGenTextures(1, &m_textureId));

        GL_ASSERT(glBindTexture(target, m_textureId));
        GL_ASSERT(glTexImage2D(target, 0, internalFormat, w, h, 0, format, dataType, data));

        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter));

        GL_ASSERT(glGenerateMipmap(target));

        m_format = format;
        m_width  = w;
        m_height = h;
    }

    void Engine::Texture::Generate(uint w, uint h, uint d, uint format, void* data)
    {
        CORE_ASSERT(target == GL_TEXTURE_3D, "Wrong texture target");

        if (!glIsTexture(m_textureId)) GL_ASSERT(glGenTextures(1, &m_textureId));
        GL_ASSERT(glBindTexture(target, m_textureId));
        GL_ASSERT(glTexImage3D(target, 0, internalFormat, w, h, d, 0, format, dataType, data));

        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter));

        GL_ASSERT(glGenerateMipmap(target));

        m_format = format;
        m_width  = w;
        m_height = h;
        m_depth  = d;
    }

    void Engine::Texture::GenerateCube(uint w, uint h, uint format, void** data)
    {
        CORE_ASSERT(target == GL_TEXTURE_CUBE_MAP, "Wrong texture target");
        if (!glIsTexture(m_textureId)) GL_ASSERT(glGenTextures(1, &m_textureId));
        GL_ASSERT(glBindTexture(target, m_textureId));

        // FIXME Type was forced to GL_Scalar, check calls for this method.
        for (int i = 0; i < 6; ++i)
        {
            GL_ASSERT(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, w, h, 0, format, dataType, data[i]));
        }

        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, minFilter));
        GL_ASSERT(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, magFilter));
        GL_ASSERT(glGenerateMipmap(target));

        m_format = format;
        m_width = w;
        m_height = h;
    }

    void Engine::Texture::bind( int unit )
    {
        if (unit >= 0)
        {
            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + unit));
        }

        GL_ASSERT(glBindTexture(target, m_textureId));
    }

    void Engine::Texture::deleteGL()
    {
        if (glIsTexture(m_textureId))
        {
            GL_ASSERT( glDeleteTextures( 1, &m_textureId ) );
        }
    }

    void Engine::Texture::updateData( void* data )
    {
        GL_ASSERT(glBindTexture(target, m_textureId));

        switch (target)
        {
            case GL_TEXTURE_1D:
            {
                GL_ASSERT(glTexImage1D(target, 0, internalFormat, m_width, 0, m_format, dataType, data));
            }

            case GL_TEXTURE_2D:
            {
                GL_ASSERT(glTexImage2D(target, 0, internalFormat, m_width, m_height, 0, m_format, dataType, data));
            } break;

            case GL_TEXTURE_3D:
            {
                GL_ASSERT(glTexImage3D(target, 0, internalFormat, m_width, m_height, m_depth, 0, m_format, dataType, data));
            } break;

            default:
            {
                CORE_ASSERT( 0, "Dafuck ?" );
            } break;
        }
    }
} // namespace Ra
