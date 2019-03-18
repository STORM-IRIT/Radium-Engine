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
 * \todo (for Radium-V2) Allow to share the same image data between different
 *       instances of a Texture.
 *       Instances could be differentiated by the sampler parameter and the
 *       mip-map availability.
 */
// FIXME: should go in Engine/Managers
class RA_ENGINE_API TextureManager final {
    RA_SINGLETON_INTERFACE( TextureManager );

  private:
    using TexturePair = std::pair<std::string, Texture*>;

  public:
    /**
     * Add a Texture giving its name, dimension and content.
     * Usefull for defining procedural Textures.
     * \param name  name of the Texture.
     * \param width width of the Texture.
     * \param height height of the Texture.
     * \param data pointer to the Texture content.
     *
     * \return a Texture descriptor that could be further specialized (filtering parameters ..)
     *         before the Texture is inserted into Radium OpenGL system by getOrLoadTexture().
     */
    TextureParameters& addTexture( const std::string& name, uint width, uint height, void* data );

    /**
     * Create a Texture, initialize the OpenGL part for it and add the created
     * Texture to the Texture cache of the engine.
     *
     * If image data are not present in \p texParameters.texels (this field is nullptr),
     * this method will assume that the \p texParameters.name field contains the
     * fully qualified filename to be loaded to initialize \p texParameters.texels.
     *
     * If image data are present in \p texParameters.texels (this field is not nullptr),
     * the name could be of any form as no loading will occur.
     *
     * \param texParameters The description of the Texture to create.
     * \param linearize true if the Texture data (\p texParameters.texels) must be
     *        converted from sRGB to LinearRGB.
     *
     * \return The Texture as inserted into the Radium available OpenGL system.
     *
     * \note For the moment, the Texture cache is indexed by the name of the Texture only.
     */
    Texture* getOrLoadTexture( const TextureParameters& texParameters, bool linearize = false );

    /**
     * Delete a named Texture from the manager.
     */
    void deleteTexture( const std::string& filename );

    /**
     * Delete a Texture from the manager.
     */
    void deleteTexture( Texture* texture );

    /**
     * Lazy update of the Texture content from the raw pointer to the new content.
     * The real update will be done when calling updatePendingTextures().
     * \note User must ensure that the data pointed by content are of the good
     *       type w.r.t.\ the Texture.
     */
    void updateTextureContent( const std::string& texture, void* content );

    /**
     * Update all Textures that are pending after a call to updateTextureContent.
     *
     * The cooperation of updateTextureContent() and updatePendingTextures() allow
     * applications to manage efficiently the on line texture generation by
     * separating the content definition (updateTextureContent()) from the
     * OpenGL state modification (updatePendingTextures()).
     */
    void updatePendingTextures();

  private:
    TextureManager();

    ~TextureManager();

    /**
     * Load a Texture as described by \p texParameters.
     * \param texParameters parameters describing the Texture to laod.
     *        This paremeters will be updated (width, height, ...) according to
     *        the loaded file properties.
     * \note Only loads 2D image file for now.
     */
    void loadTexture( TextureParameters& texParameters );

  private:
    /// The list of Textures.
    std::map<std::string, Texture*> m_textures;

    /// The list of TextureData for Textures not yet loaded.
    std::map<std::string, TextureParameters> m_pendingTextures;

    /// The list of Texture data for Textures not yet loaded.
    std::map<std::string, void*> m_pendingData;

    /// Whether to print stat info to the Info output.
    bool m_verbose;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTUREMANAGER_HPP
