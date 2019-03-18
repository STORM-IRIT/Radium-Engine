#ifndef RADIUMENGINE_TEXTURE_HPP
#define RADIUMENGINE_TEXTURE_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <string>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace globjects {
class Texture;
}

namespace Ra {
namespace Engine {

/**
 * Describes the content and parameters of a Texture.
 * This structures encapsulates all the states used for creating an OpenGL texture.
 * These parameters describe the image data of the Texture:
 *   - target, width, height, depth, format, internalFormat, type and texels
 *     for describing image data;
 *   - wrapS, wrapT, wrapP, minfilter and magFilter for describing the sampler
 *     of the texture.
 *
 * When one wants to create a Texture, the first thing to do is to create and
 * fill a TextureParameters structure that will describe the Texture.
 *
 * The Texture creation could be done either using the TextureManager or
 * directly on the client class/function.
 *
 * When a Texture is created, no OpenGL initialisation is realized.
 * The user must first call initializeGL before being able to use this Texture
 * in an OpenGL operation.
 *
 * MipMap representation of the Texture is automatically generated as soon as the
 * minFilter parameter is something else than GL_LINEAR or GL_NEAREST.
 *
 * \note No coherence checking will be done on the content of this structure.
 *       User must ensure coherent data and parameters before creating the
 *       OpenGL texture with Texture::initializeGL.
 */
struct TextureParameters {
    /// Name of the Texture.
    std::string name{};

    /// OpenGL target.
    GLenum target{GL_TEXTURE_2D};

    /// width of the Texture (s dimension).
    size_t width{1};

    /// height of the Texture (t dimension).
    size_t height{1};

    /// width of the Texture (p dimension).
    size_t depth{1};

    /// Format of the external data.
    GLenum format{GL_RGB};

    /// OpenGL internal format (WARNING, for Integer textures, must be GL_XXX_INTEGER).
    GLenum internalFormat{GL_RGB};

    /// Type of the components in external data.
    GLenum type{GL_UNSIGNED_BYTE};

    /// OpenGL wrap mode in the s direction.
    GLenum wrapS{GL_CLAMP_TO_EDGE};

    /// OpenGL wrap mode in the t direction.
    GLenum wrapT{GL_CLAMP_TO_EDGE};

    /// OpenGL wrap mode in the p direction.
    GLenum wrapP{GL_CLAMP_TO_EDGE};

    /// OpenGL minification filter ( GL_LINEAR or GL_NEAREST or GL_XXX_MIPMAP_YYY ).
    GLenum minFilter{GL_LINEAR};

    /// OpenGL magnification filter ( GL_LINEAR or GL_NEAREST ).
    GLenum magFilter{GL_LINEAR};

    /// External data (not stored after OpenGL texture creation).
    void* texels{nullptr};
};

/**
 * Represents a Texture of the RadiumEngine.
 * \see TextureManager to informations about how unique Textures are defined.
 */
class RA_ENGINE_API Texture final {
  public:
    /**
     * Copy operator is forbidden.
     */
    Texture( const Texture& ) = delete;

    /**
     * Assignment operator is forbidden.
     */
    void operator=( const Texture& ) = delete;

    /**
     * Texture constructor. No OpenGL initialization is done there.
     * \param texParameters Name of the Texture.
     */
    explicit Texture( const TextureParameters& texParameters );

    /**
     * Texture destructor. Both internal data and OpenGL stuff are deleted.
     */
    ~Texture();

    /**
     * \brief Generate the OpenGL representation of the texture according to the stored
     * TextureParameters.
     *
     * This method use the available m_textureParameters to generate and configure OpenGL
     * texture.
     *
     * Before uploading texels to the GPU, this method will apply RGB space conversion if needed.
     *
     * \param linearize (default false): if true, converts the Texture from sRGB
     *        to Linear RGB color space before OpenGL initialisation.
     * \note This will become soon the only way to generate an OpenGL texture in Radium.
     */
    void initializeGL( bool linearize = false );

    /**
     * \brief Init the Textures needed for the cubemap from OpenGL point of view.
     *
     * Generate, bind and configure OpenGL texture.
     * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR,
     * although no mipmaps are generated.
     *
     * It is highly recommended to take a look at
     * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage2D">glTexImage2D documentation</a>
     * since this method doc will highly refer to it.
     *
     * \param width Width of the six 2D Textures.
     * \param height Height of the six 2D Textures.
     * \param format The format of the pixel data.
     *        Refer to the link given above, at the \p format section
     *        for further informations about the available formats.
     * \param data Data contained in the Texture. Can be nullptr.
     *        If \p data is not null, the Texture will take the ownership of it.
     * \param linearize (default false): if true, converts the Texture from sRGB
     *                                   to Linear RGB color space.
     * \param mipmaped (default false): generate a prefiltered mipmap for the Texture.
     *
     * \todo Integrate this method in the same workflow than other Textures...
     */
    void generateCube( uint width, uint height, GLenum format, void** data = nullptr,
                       bool linearize = false, bool mipmaped = false );

    /**
     * \brief Bind the Texture to enable its use in a shader.
     * \param unit Index of the Texture to be bound. If -1 only calls glBindTexture.
     */
    void bind( int unit = -1 );

    /**
     * Bind the Texture to an image unit for the purpose of reading and writing it from shaders.
     * Uses m_parameters.internalFormat as format.
     * \see
     * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml
     * for documentation.
     * \note Only available since openGL 4.2, not available on MacOs.
     */
    void bindImageTexture( int unit, const GLint level, const GLboolean layered, const GLint layer,
                           const GLenum access );

    /**
     * Return Name of the Texture.
     */
    inline std::string getName() const { return m_textureParameters.name; }

    /**
     * Update the data contained by the Texture.
     * \note \p newData must contain the same number of elements than the old
     *       data, no check will be performed.
     */
    void updateData( void* newData );

    /**
     * Update the parameters contained by the Texture.
     * User first modifies the public attributes corresponding to the parameter
     * he wants to change the value (e.g wrap* or *Filter) and then calls this
     * function to update the OpenGL texture state.
     */
    void updateParameters();

    /**
     * Convert a color Texture from sRGB to Linear RGB spaces.
     * This will transform the internal representation of the texture to GL_FLOAT.
     * Only GL_RGB[8, 16, 16F, 32F] and GL_RGBA[8, 16, 16F, 32F] are managed.
     * Full transformation as described at https://en.wikipedia.org/wiki/SRGB.
     * \param gamma the gama value to use (sRGB is 2.4).
     */
    void linearize( Scalar gamma = Scalar( 2.4 ) );

    /**
     * Return the pixel format of the Texture.
     */
    GLenum format() const { return m_textureParameters.format; }

    /**
     * Return the width of the Texture.
     */
    size_t width() const { return m_textureParameters.width; }

    /**
     * Return the height of the Texture.
     */
    size_t height() const { return m_textureParameters.height; }

    /**
     * Return the depth of the Texture.
     */
    size_t depth() const { return m_textureParameters.depth; }

    /**
     * Return the texels of the Texture.
     */
    void* texels() { return m_textureParameters.texels; }

    /**
     * Return the globjects::Texture associated with the Texture.
     */
    globjects::Texture* texture() const { return m_texture.get(); }

    /**
     * Resize the Texture.
     * This allocate GPU memory to store the new resized Texture and,
     * if texels are not nullptr, upload the new content.
     * \param w width of the Texture.
     * \param h height of the Texture.
     * \param d depth of the Texture.
     * \param pix the new texel array corresponding the the new Texture dimension.
     * \note If texels are not nullptr, user must ensure the texels array is
     *       correctly dimensionned.
     */
    void resize( size_t w = 1, size_t h = 1, size_t d = 1, void* pix = nullptr );

  public:
    /// Texture parameters.
    TextureParameters m_textureParameters;

  private:
    /**
     * Convert a color Texture from sRGB to Linear RGB spaces.
     * The content of the array of texel designated by the texel pointer is
     * modified by side effect.
     * Full transformation as described at https://en.wikipedia.org/wiki/SRG.B
     * \param texels the array of texels to linearize.
     * \param numCommponent number of color channels.
     * \param hasAlphaChannel indicate if the last channel is an alpha channel.
     * \param gamma the gama value to use (sRGB is 2.4).
     * \note Only 8 bit (GL_UNSIGNED_BYTE data format) textures are managed by this operator.
     */
    void sRGBToLinearRGB( uint8_t* texels, uint numCommponent, bool hasAlphaChannel,
                          Scalar gamma = Scalar( 2.4 ) );

  private:
    /// Link to glObject texture.
    std::unique_ptr<globjects::Texture> m_texture;

    /// Whether the Texture is mipmaped.
    bool m_isMipMaped{false};

    /// Whether the Texture is in LinearRGB.
    bool m_isLinear{false};
};
} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTURE_HPP
