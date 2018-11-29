#ifndef RADIUMENGINE_TEXTUREMANAGER_HPP
#define RADIUMENGINE_TEXTUREMANAGER_HPP

#include <Engine/RaEngine.hpp>
#include <map>
#include <string>

#include <Core/Utils/Singleton.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
namespace Ra {
namespace Engine {


/**
 * Manage Texture loading and registration.
 * @todo (for Radium-V2) Allow to share the same texel data between different instances of a texture.
 * Instances could be differentiated by the sampler parameter and the mip-map availability.
 *
 */
class RA_ENGINE_API TextureManager final {
    RA_SINGLETON_INTERFACE( TextureManager );

  private:
    using TexturePair = std::pair<std::string, Texture*>;

  public:
    /** Add a texture giving its name, dimension and content.
     * Usefull for defining procedural textures
     */
    TextureData& addTexture(const std::string &name, uint width, uint height, void *data);

    /**
     * Get or load a named texture.
     * The name of the texture might be different of the associated file but the data must be
     * loaded in the TextureData before calling this method.
     * @param filename : file to load
     * @param linearize : true if the texture must be converted from sRGB to LinearRGB
     * @return
     */
    Texture *getOrLoadTexture(const TextureData &data, bool linearize = false);

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

    /**
     * Lazy update the texture content from the raw pointer content.
     * The real update will be done when calling updatePendingTextures
     * @note User must ensure that the data pointed by content are of the good type wrt the texture.
     * @param texture
     * @param content
     */
    void updateTextureContent(const std::string &texture, void *content);

    /**
     * Update all textures that are pending after a call to updateTextureContent.
     *
     * The cooperation of updateTextureContent and updatePendingTextures allow applications to manage efficiently
     * the on line texture generation by separating the content definition (updateTextureContent)
     * from the OpenGL state modification (updatePendingTextures).
     */
    void updatePendingTextures();

  private:
    TextureManager();
    ~TextureManager();

    /** Load a texture as described by texParameters.
    * @note : only loads 2D image file for now.
    * @param texParameters parameters describing the texture to laod. This paremeters will be updated
     * (width, height, ...) according to the loaded file properties.
    */
    void loadTexture( TextureData& texParameters );

private:
    std::map<std::string, Texture*> m_textures;
    std::map<std::string, TextureData> m_pendingTextures;
    std::map<std::string, void*> m_pendingData;

    bool m_verbose;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTUREMANAGER_HPP
