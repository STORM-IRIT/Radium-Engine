#pragma once

#include <Engine/RaEngine.hpp>
#include <map>
#include <string>

#include <Engine/Data/Texture.hpp>
#include <Engine/OpenGL.hpp>
namespace Ra {
namespace Engine {
namespace Data {

/**
 * Manage Texture loading and registration.
 * \todo (for Radium-V2) Allow to share the same image data between different instances of a
 * texture. Instances could be differentiated by the sampler parameter and the mip-map availability.
 */
class RA_ENGINE_API TextureManager final
{

  private:
    using TexturePair = std::pair<std::string, Texture*>;

  public:
    /** Add a texture giving its name, dimension and content.
     * Useful for defining procedural textures
     *
     * \param name  name of the texture
     * \param width width of the texture
     * \param height height of the texture
     * \param data pointer to the texture content
     *
     * \return a texture descriptor that could be further specialized (filtering parameters ..)
     * before the texture is inserted into Radium OpenGL system by getOrLoadTexture
     */
    TextureParameters&
    addTexture( const std::string& name, uint width, uint height, std::shared_ptr<void> data );

    /**
     * Get or load a named texture.
     * If image data are not presents in texParameters.texels (this field is nullptr), this method
     * will assume that the texParameters.name field contains the fully qualified filename to be
     * loaded to initialize texParameters.texels
     *
     * If image data are presents in texParameters.texels (this field is not nullptr), the name
     * could be of any form as no loading will occur.
     *
     *
     * This method creates, initialize OpenGL part of the texture and add the created texture to the
     * Texture cache of the engine.
     * \note For the moment, the texture cache is indexed by the name of the texture only.
     *
     * \param texParameters : The description of the texture to create
     * \param linearize : true if the texture data (texParameters.texels) must be converted from
     * sRGB to LinearRGB
     * \return The texture as inserted into the Radium available openGL system
     */
    Texture* getOrLoadTexture( const TextureParameters& texParameters, bool linearize = false );

    /**
     * Helper function, load the texture without adding it to the manager.
     * \see getOrLoadTexture() for parameters description.
     */
    Texture* loadTexture( const TextureParameters& texParameters, bool linearize = false );

    /**
     * Delete a named texture from the manager
     * \param filename
     */
    void deleteTexture( const std::string& filename );
    /**
     * Delete a texture from the manager
     * \param texture
     */
    void deleteTexture( Texture* texture );

    /**
     * Lazy update the texture content from the raw pointer content.
     * The real update will be done when calling updatePendingTextures
     * \note User must ensure that the data pointed by content are of the good type wrt the texture.
     * \param texture
     * \param content
     */
    void updateTextureContent( const std::string& texture, std::shared_ptr<void> content );

    /**
     * Update all textures that are pending after a call to updateTextureContent.
     *
     * The cooperation of updateTextureContent and updatePendingTextures allow applications to
     * manage efficiently the on line texture generation by separating the content definition
     * (updateTextureContent) from the OpenGL state modification (updatePendingTextures).
     *
     * \todo find a better name and description for this method that do not act on
     * _pending textures_, i.e. textures that do not have a valid OpenGl state.
     */
    void updatePendingTextures();

    /** Load a texture as described by texParameters.
     * \note : only loads 2D image file for now.
     * \param texParameters parameters describing the texture to load. This parameter will be
     * updated (width, height, ...) according to the loaded file properties.
     */
    void loadTextureImage( TextureParameters& texParameters );

  public:
    TextureManager();
    ~TextureManager();

  private:
    /// Textures that have a usable and up to date OpenGL state
    std::map<std::string, Texture*> m_textures;
    /// Textures that do not have a usable OpenGL state
    std::map<std::string, TextureParameters> m_pendingTextures;
    /// Textures whose OpenGl stat is not up to date
    std::map<std::string, std::shared_ptr<void>> m_pendingData;
};

} // namespace Data
} // namespace Engine
} // namespace Ra
