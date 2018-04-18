#ifndef RADIUMENGINE_TEXTUREMANAGER_HPP
#define RADIUMENGINE_TEXTUREMANAGER_HPP

#include <Engine/RaEngine.hpp>
#include <map>
#include <string>

#include <Core/Utils/Singleton.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra {
namespace Engine {
class Texture;
}
} // namespace Ra

namespace Ra {
namespace Engine {
struct TextureData {
    std::string name;
    int width;
    int height;

    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB;
    GLenum type = GL_UNSIGNED_BYTE;

    GLenum wrapS = GL_CLAMP_TO_EDGE;
    GLenum wrapT = GL_CLAMP_TO_EDGE;

    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLenum magFilter = GL_LINEAR;

    void* data = nullptr;
};

class RA_ENGINE_API TextureManager final {
    RA_SINGLETON_INTERFACE( TextureManager );

  private:
    using TexturePair = std::pair<std::string, Texture*>;

  public:
    TextureData& addTexture( const std::string& name, int width, int height, void* data );
    Texture* addTexture( const std::string& filename );
    Texture* getOrLoadTexture( const std::string& filename );
    Texture* getOrLoadTexture( const TextureData& data );

    void deleteTexture( const std::string& filename );
    void deleteTexture( Texture* texture );

    // Call this method to update given texture
    void updateTexture( const std::string& texture, void* data );

    // Called by materials
    void updateTextures();

  private:
    TextureManager();
    ~TextureManager();

  private:
    std::map<std::string, Texture*> m_textures;
    std::map<std::string, TextureData> m_pendingTextures;
    std::map<std::string, void*> m_pendingData;

    bool m_verbose;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTUREMANAGER_HPP
