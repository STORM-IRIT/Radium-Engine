#ifndef RADIUMENGINE_FBO_HPP
#define RADIUMENGINE_FBO_HPP

#include <map>

#include <Core/CoreMacros.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra { namespace Engine { class Texture; } }

namespace Ra { namespace Engine {

class Texture;

class FBO
{
public:
    enum Components
    {
        COLOR = 1 << 0,
        DEPTH = 1 << 1,
        STENCIL = 1 << 2,
        ALL_COMPONENTS = COLOR | DEPTH | STENCIL
    };

    FBO(Components components, uint width, uint height);
    ~FBO();

    void bind();

    // Binds and sets viewport
    void useAsTarget();
    void useAsTarget(uint width, uint height);
    void unbind(bool complete = false);

    void attachTexture(uint attachment, Texture* texture, uint level = 0);
    void detachTexture(uint attachment);

    void check() const;

    void setSize(uint width, uint height);
    void clear(Components components);

    uint getID() const { return m_fboID; }

private:
    FBO(const FBO&) = delete;
    void operator=(const FBO&) = delete;

private:
    uint m_fboID;
    Components m_components;
    uint m_width, m_height;

    std::map<uint, Texture*> m_textures;

    bool m_isBound;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FBO_HPP
