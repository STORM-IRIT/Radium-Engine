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
/**
 * Describes the content and parameters of a texture.
 */
struct TextureData {
    std::string name;
    int width = -1;
    int height = -1;

    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB;
    GLenum type = GL_UNSIGNED_BYTE;

    GLenum wrapS = GL_CLAMP_TO_EDGE;
    GLenum wrapT = GL_CLAMP_TO_EDGE;

    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLenum magFilter = GL_LINEAR;

    void* data = nullptr;
};

/**
 * Manage Texture loading and registration.
 */
class RA_ENGINE_API TextureManager final {
    RA_SINGLETON_INTERFACE( TextureManager );

  private:
    using TexturePair = std::pair<std::string, Texture*>;

  public:
    /** Add a texture giving its name, dimension and content.
     * Usefull for defining procedural textures
     */
    TextureData& addTexture( const std::string& name, int width, int height, void* data );

    /**
     * Get or load texture from a file.
     * The name of the texture is the name of its file
     * @param filename
     * @return
     */
    Texture* getOrLoadTexture( const std::string& filename );

    /**
     * Get or load a named texture.
     * The name of the texture might be different of the associated file but the data must be
     * loaded in the TextureData before calling this method.
     * @param filename
     * @return
     */
    Texture* getOrLoadTexture( const TextureData& data );

    /**
     * Delete a named texture from the manager
     * @param filename
     */
    void deleteTexture( const std::string& filename );
    /**
     * Delete a texture from the manager
     * @param texture
     */
    void deleteTexture( Texture* texture );

    /// Call this method to update given texture
    void updateTexture( const std::string& texture, void* data );

    /// Called by materials
    void updateTextures();

  private:
    TextureManager();
    ~TextureManager();

    /** Load a given filename and return the associated TextureData.
    * @note : only loads 2D image file for now.
    * @param filename
    * @return
    */
    TextureData loadTexture( const std::string& filename );

private:
    std::map<std::string, Texture*> m_textures;
    std::map<std::string, TextureData> m_pendingTextures;
    std::map<std::string, void*> m_pendingData;

    bool m_verbose;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTUREMANAGER_HPP
