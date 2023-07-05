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
    Texture* getTexture( const TextureHandle& handle );
    TextureHandle getTextureHandle( const std::string& name );
    Texture* getTexture( const std::string& name ) {
        return getTexture( getTextureHandle( name ) );
    }

    void deleteTexture( const TextureHandle& handle );

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
    std::vector<std::unique_ptr<Texture>> m_newTextures;
};

} // namespace Data
} // namespace Engine
} // namespace Ra
