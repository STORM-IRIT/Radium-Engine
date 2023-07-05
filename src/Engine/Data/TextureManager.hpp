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
  public:
    using TextureHandle = Ra::Core::Utils::Index;

    /** Add a texture giving its name, dimension and content.
     * Useful for defining procedural textures
     *
     * \param name name of the texture
     * \param width width of the texture
     * \param height height of the texture
     * \param data pointer to the texture content
     *
     * \return a texture descriptor that could be further specialized (filtering parameters ..)
     * before the texture is inserted into Radium OpenGL system by getOrLoadTexture
     */
    TextureParameters&
    addTexture2( const std::string& name, uint width, uint height, std::shared_ptr<void> data );

    TextureHandle addTexture( const TextureParameters& );

    /** \brief Get or load a named texture.
     *
     * If image texParameters.texels is nullptr, this method
     * will assume that the texParameters.name field contains the fully qualified filename to be
     * loaded to initialize texParameters.texels
     *
     * If texParameters.texels isn't nullptr, the name could be of any form as no loading will
     * occur.
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
    Texture* getOrLoadTexture2( const TextureParameters& texParameters, bool linearize = false );
    Texture* getTexture( const TextureHandle& handle );
    TextureHandle getTextureHandle( const std::string& handle );

    /**
     * Helper function, load the texture without adding it to the manager.
     * \see getOrLoadTexture() for parameters description.
     */
    Texture* loadTexture2( const TextureParameters& texParameters, bool linearize = false );

    /**
     * Delete a named texture from the manager
     * \param filename
     */
    void deleteTexture2( const std::string& filename );

    /**
     * Delete a texture from the manager
     * \param texture
     */
    void deleteTexture2( Texture* texture );
    void deleteTexture( const TextureHandle& handle );
    /**
     * Lazy update the texture content from the raw pointer content.
     * The real update will be done when calling RadiumEngine::runGpuTasks() (e.g. during next
     * BaseApplication::radiumFrame() )
     * \note User must ensure that the data pointed by content are of the good type wrt the texture.
     * \param texture
     * \param content
     */
    void updateTextureContent( const std::string& texture, std::shared_ptr<void> content );

    /** Load \a filename and fill ImageParameters according to \a filename content.
     * \note : only loads 2D image file for now.
     * \param filename fully qualified image filename. See stbi_load_image for supported file
     * format.
     * \return ImageParameters corresponding to filename content, image.parameters.texels contains
     * actual image data. In case of loading error, texels is nullptr, width = height = 0.
     */
    static ImageParameters loadTextureImage( const std::string& filename, bool linearize = false );

  public:
    TextureManager();
    ~TextureManager();

  private:
    /// Textures that have a usable and up to date OpenGL state
    std::map<std::string, Texture*> m_textures;
    std::vector<std::unique_ptr<Texture>> m_newTextures;

    /// Textures that do not have a usable OpenGL state
    std::map<std::string, TextureParameters> m_pendingTextures;

    /// Handle to filename correspondance, if texture image is loaded from file.
    std::map<TextureHandle, std::string> m_filenames;
};

} // namespace Data
} // namespace Engine
} // namespace Ra
