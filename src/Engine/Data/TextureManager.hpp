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

    /** Add a texture given its TextureParameters
     * \return a TextureHandle that uniquely identify the texture in the manager.
     */
    TextureHandle addTexture( const TextureParameters& p );

    /** Get texture ptr from handle.
     *
     * Please do not delete the return ptr.
     */

    Texture* getTexture( const TextureHandle& handle );

    /** Get a texture handle from textue name.
     *
     * It search for the first texture in the set of managed texture that have this name.
     * \return An handle to the texture or InvalidIndex if no texture with this name is found
     */
    TextureHandle getTextureHandle( const std::string& name );

    /** Convinience function to obtain Texture ptr from name */
    Texture* getTexture( const std::string& name ) {
        return getTexture( getTextureHandle( name ) );
    }

    /** Remove a managed texture identified by handle.
     *
     * Texture dtor will register the delete texture gpu task.
     */
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
    TextureManager( TextureManager const& )            = delete;
    TextureManager( TextureManager&& )                 = delete;
    TextureManager& operator=( TextureManager const& ) = delete;
    TextureManager& operator=( TextureManager&& )      = delete;

  private:
    std::vector<std::unique_ptr<Texture>> m_textures;
};

} // namespace Data
} // namespace Engine
} // namespace Ra
