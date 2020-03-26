#ifndef RADIUMENGINE_TEXTURE_HPP
#define RADIUMENGINE_TEXTURE_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <string>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>

#include <Core/Utils/Color.hpp>

namespace globjects {
class Texture;
}

namespace Ra {
namespace Engine {

/**
 * Describes the content and parameters of a texture.
 * This structures encapsulates all the states used for creating an OpenGL texture.
 *  These parameters describe the image data of the texture :
 *    - target, width, height, depth, format, internalFormat, type and texels for describing image
 * data
 *    - wrapS, wrapT, wrapP, minfilter and magFilter for describing the sampler of the texture.
 *
 *  When one wants to create a texture, the first thing to do is to create and fill a Texture
 * parameter structure that will describe the Texture.
 *
 *  The Texture creation could be done either using the TextureManager or directly on the client
 * class/function.
 *
 *  When a texture is created, no OpenGL initialisation is realized. The user must first call
 * initializeGL before being able to use this texture in an OpenGL operation. initializeGL
 *
 *  MipMap representation of the texture is automatically generated as soon as the minFilter
 * parameter is something else than GL_LINEAR or GL_NEAREST
 *
 * @note No coherence checking will be done on the content of this structure. User must ensure
 * coherent data and parameters before creating the OpenGL texture with Texture::initializeGL
 */
struct TextureParameters {
    /// Name of the texture
    std::string name{};
    /// OpenGL target
    GLenum target{GL_TEXTURE_2D};
    /// width of the texture (s dimension)
    size_t width{1};
    /// height of the texture (t dimension)
    size_t height{1};
    /// width of the texture (p dimension)
    size_t depth{1};
    /// Format of the external data
    GLenum format{GL_RGB};
    /// OpenGL internal format (WARNING, for Integer textures, must be GL_XXX_INTEGER)
    GLenum internalFormat{GL_RGB};
    /// Type of the components in external data
    GLenum type{GL_UNSIGNED_BYTE};
    /// OpenGL wrap mode in the s direction
    GLenum wrapS{GL_CLAMP_TO_EDGE};
    /// OpenGL wrap mode in the t direction
    GLenum wrapT{GL_CLAMP_TO_EDGE};
    /// OpenGL wrap mode in the p direction
    GLenum wrapP{GL_CLAMP_TO_EDGE};
    /// OpenGL minification filter ( GL_LINEAR or GL_NEAREST or GL_XXX_MIPMAP_YYY )
    GLenum minFilter{GL_LINEAR};
    /// OpenGL magnification filter ( GL_LINEAR or GL_NEAREST )
    GLenum magFilter{GL_LINEAR};
    /// External data (ownership is left to caller, not stored after OpenGL texture creation).
    /// Note that, for cubmap texture, this is considered as a "void*[6]" array containing the 6
    /// faces of the cube corresponding to the targets. <br/>
    /// texels[0] <-- GL_TEXTURE_CUBE_MAP_POSITIVE_X <br/>
    /// texels[1] <-- GL_TEXTURE_CUBE_MAP_NEGATIVE_X <br/>
    /// texels[2] <-- GL_TEXTURE_CUBE_MAP_POSITIVE_Y <br/>
    /// texels[3] <-- GL_TEXTURE_CUBE_MAP_NEGATIVE_Y <br/>
    /// texels[4] <-- GL_TEXTURE_CUBE_MAP_POSITIVE_Z <br/>
    /// texels[5] <-- GL_TEXTURE_CUBE_MAP_NEGATIVE_Z <br/>
    void* texels{nullptr};
};

/** Represent a Texture of the engine
 * See TextureManager to informations about how unique texture are defined.
 */
class RA_ENGINE_API Texture final
{
  public:
    /** Texture parameters
     */
    TextureParameters m_textureParameters;

    /** Textures are not copyable, delete copy constructor.
     */
    Texture( const Texture& ) = delete;

    /** Textures are not copyable, delete operator =.
     */
    void operator=( const Texture& ) = delete;

    /**
     * Texture constructor. No OpenGL initialization is done there.
     *
     * @param texParameters Name of the texture
     */
    explicit Texture( const TextureParameters& texParameters );

    /**
     * Texture destructor. Both internal data and OpenGL stuff are deleted.
     */
    ~Texture();

    /** @brief Generate the OpenGL representation of the texture according to the stored TextureData
     *
     * This method use the available m_textureParameters to generate and configure OpenGL
     * texture.
     *
     * Before uploading texels to the GPU, this method will apply RGB space conversion if needed.
     *
     * @param linearize (default false) : convert the texture from sRGB to Linear RGB color space
     * before OpenGL initialisation
     * @note This will become soon the only way to generate an Radium Engine OpenGL texture.
     */
    void initializeGL( bool linearize = false );

    /**
     * @brief Bind the texture to enable its use in a shader
     * @param unit Index of the texture to be bound. If -1 only calls glBindTexture.
     */
    void bind( int unit = -1 );

    /**
     * Bind the texture to an image unit for the purpose of reading and writing it from shaders.
     * @note, only available since openGL 4.2, not available on MacOs
     * uses m_parameters.internalFormat as format.
     * see
     * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml
     * for documentation
     */
    void bindImageTexture( int unit,
                           const GLint level,
                           const GLboolean layered,
                           const GLint layer,
                           const GLenum access );

    /**
     * @return Name of the texture.
     */
    inline std::string getName() const { return m_textureParameters.name; }

    /**
     * Update the data contained by the texture
     * @param newData The new data, must contain the same number of elements than old data, no
     * check will be performed.
     */
    void updateData( void* newData );

    /**
     * Update the parameters contained by the texture.
     * User first modify the public attributes corresponding to the parameter he wants to change
     * the value (e.g wrap* or *Filter) and call this function to update the OpenGL texture
     * state ...
     * @return
     */
    void updateParameters();

    /**
     * Convert a color texture from sRGB to Linear RGB spaces.
     * This will transform the internal representation of the texture to GL_FLOAT.
     * Only GL_RGB[8, 16, 16F, 32F] and GL_RGBA[8, 16, 16F, 32F] are managed.
     * Full transformation as described at https://en.wikipedia.org/wiki/SRGB
     */
    void linearize();

    /**
     * @return the pixel format of the texture
     */
    GLenum format() const { return m_textureParameters.format; }
    /**
     * @return the width of the texture
     */
    size_t width() const { return m_textureParameters.width; }
    /**
     * @return the height of the texture
     */
    size_t height() const { return m_textureParameters.height; }
    /**
     * @return the depth of the texture
     */
    size_t depth() const { return m_textureParameters.depth; }

    void* texels() { return m_textureParameters.texels; }
    /**
     * @return the globjects::Texture associated with the texture
     */
    globjects::Texture* texture() const { return m_texture.get(); }

    /** Resize the texture.
     * This allocate GPU memory to store the new resized texture and, if texels are not nullptr,
     * upload the new content.
     * @note : If texels are not nullptr, user must ensure the texels array is correctly
     * dimmensionned.
     * @param w width of the texture
     * @param h height of the texture
     * @param d depth of the texture
     * @param pix the new texel array corresponding the the new texture dimension
     */
    void resize( size_t w = 1, size_t h = 1, size_t d = 1, void* pix = nullptr );

  private:
    /**
     * Convert a color texture from sRGB to Linear RGB spaces.
     * The content of the array of texel
     * designated by the texel pointer is modified by side effect.
     * Full transformation as described at https://en.wikipedia.org/wiki/SRGB
     * @param texels the array of texels to linearize
     * @param numCommponent number of color channels.
     * @param bool hasAlphaChannel indicate if the last channel is an alpha channel.
     * @param gamma the gama value to use (sRGB is 2.4)
     * @note only 8 bit (GL_UNSIGNED_BYTE data format) textures are managed by this operator.
     */
    void sRGBToLinearRGB( uint8_t* texels, uint numCommponent, bool hasAlphaChannel );

    /// linearize a cube map by calling sRGBToLinearRGB fore each face
    void linearizeCubeMap( uint numCommponent, bool hasAlphaChannel );

    /// Link to glObject texture
    std::unique_ptr<globjects::Texture> m_texture;
    /// Is the texture mipmaped ?
    bool m_isMipMaped{false};
    /// Is the texture in LinearRGB ?
    bool m_isLinear{false};

#if 0
    /**
  * @brief Init the textures needed for the cubemap from OpenGL point of view.
  *
  * Generate, bind and configure OpenGL texture.<br/>
  * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are
  * generated.<br/><br/>
  *
  * It is highly recommended to take a look at
  * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage2D">glTexImage2D documentation</a>
  * since this method doc will highly refer to it.
  *
  *
  * @param width Width of the six 2D textures.
  *
  * @param height Height of the six 2D textures.
  *
  * @param format The format of the pixel data.
  * Refer to the link given above, at the \b format section
  * for further informations about the available formats.
  *
  * @param data Data contained in the texture. Can be nullptr. <br/>
  * If \b data is not null, the texture will take the ownership of it.
  *
  * @param linearize (default false) : convert the texture from sRGB to Linear RGB color space
  *
  * @param mipmaped (default false) : generate a prefiltered mipmap for the texture.
  *
  * @todo integrate this method in the same workflow than other textures ...
  */
    void generateCube( bool linearize = false );
#endif
};
} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTURE_HPP
