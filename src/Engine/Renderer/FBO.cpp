#include <Engine/Renderer/FBO.hpp>

#include <cstdio>

#include <Engine/Renderer/Texture.hpp>

namespace Ra
{

FBO::FBO(Components components, uint width, uint height)
    : m_components(components)
    , m_width(width ? width : 1)
    , m_height(height ? height : 1)
    , m_isBound(false)
{
    GL_ASSERT(glGenFramebuffers(1, &m_fboID));
}

FBO::~FBO()
{
    GL_ASSERT(glDeleteFramebuffers(1, &m_fboID));
    m_textures.clear();
}

void FBO::bind()
{
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));
    m_isBound = true;
}

void FBO::unbind(bool complete)
{
    m_isBound = false;
    if (complete)
    {
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}

void FBO::attachTexture(uint attachment, Texture *texture, uint level)
{
    assert(m_isBound && "FBO must be bound to attach a texture.");

    switch (texture->getType())
    {
        case Texture::TEXTURE_1D:
        {
            GL_ASSERT(glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, texture->getTarget(),
                                             texture->getId(), level));
        } break;

        case Texture::TEXTURE_2D:
        case Texture::TEXTURE_CUBE:
        {
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture->getTarget(),
                                             texture->getId(), level));
        } break;

        case Texture::TEXTURE_3D:
        {
            GL_ASSERT(glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, texture->getTarget(),
                                             texture->getId(), level, texture->getZOffset()));
        } break;
    }

    m_textures[attachment] = texture;
}

void FBO::detachTexture(uint attachment)
{
    assert(m_isBound && "FBO must be bound to detach a texture.");
    assert(m_textures.find(attachment) != m_textures.end() &&
            "Trying to detach a non attached texture.");

    Texture* texture = m_textures[attachment];

    switch (texture->getType())
    {
        case Texture::TEXTURE_1D:
        {
            GL_ASSERT(glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, 0, 0, 0));
        } break;

        case Texture::TEXTURE_2D:
        case Texture::TEXTURE_CUBE:
        {
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, 0, 0, 0));
        } break;

        case Texture::TEXTURE_3D:
        {
            GL_ASSERT(glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, 0, 0, 0, 0));
        } break;
    }

    m_textures.erase(attachment);
}

void FBO::check() const
{
    // TODO: More verbose check
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (GL_FRAMEBUFFER_COMPLETE != fboStatus)
    {
        std::string err;
        switch (fboStatus)
        {
            case GL_FRAMEBUFFER_UNDEFINED:
            {
                err = "GL_FRAMEBUFFER_UNDEFINED";
            } break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            {
                err = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            } break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            {
                err = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            } break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            {
                err = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            } break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            {
                err = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            } break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
            {
                err = "GL_FRAMEBUFFER_UNSUPPORTED";
            } break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            {
                err = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            } break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            {
                err = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            } break;
            default:
            {
                char buff[100];
                snprintf(buff, 100, "Unknown %08x", fboStatus);
                err = buff;
            }
        }

        fprintf(stderr, "FBO Error : %s\n", err.c_str());
    }
    assert(fboStatus == GL_FRAMEBUFFER_COMPLETE && "Something went wrong with the Framebuffer.\n");
}

void FBO::setSize(uint width, uint height)
{
    m_width = width;
    m_height = height;
}

void FBO::clear(Components components)
{
    assert(m_isBound && "FBO must be bound before calling clear().");

    Components nc = Components(m_components & components);

    GLbitfield mask = 0;

    if (nc & COLOR)
    {
        mask |= GL_COLOR_BUFFER_BIT;
    }

    if (nc & DEPTH)
    {
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    if (nc & STENCIL)
    {
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    GL_ASSERT(glClear(mask));
}

} // namespace Ra
