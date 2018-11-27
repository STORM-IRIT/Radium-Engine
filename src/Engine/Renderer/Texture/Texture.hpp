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
  /** Represent a Texture of the engine
   * See TextureManager to informations about how unique texture are defined.
   */
class RA_ENGINE_API Texture final {
  public:

    // internal format of the OpenGL texture
    GLenum internalFormat {GL_RGB};
    GLenum dataType {GL_UNSIGNED_BYTE};
    GLenum wrapS {GL_REPEAT};
    GLenum wrapT {GL_REPEAT};
    GLenum wrapR {GL_REPEAT};
    GLenum minFilter {GL_LINEAR};
    GLenum magFilter {GL_LINEAR};

    /** Textures are not copyable, delete copy constructor.
     */
    Texture( const Texture& ) = delete;

    /** Textures are not copyable, delete operator =.
     */
    void operator=( const Texture& ) = delete;

    /**
     * Texture constructor. No OpenGL initialization is done there.
     *
     * @param name Name of the texture
     *
     */
    explicit Texture( const std::string &name = "" );

    /**
     * Texture desctructor. Both internal data and OpenGL stuff are deleted.
     */
    ~Texture();

    /**
     * @brief Init the texture 1D from OpenGL point of view.
     *
     * Generate, bind and configure OpenGL texture.<br/>
     * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are
     * generated.<br/><br/>
     *
     * It is highly recommended to take a look at
     * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage1D">glTexImage1D documentation</a>
     * since this method doc will highly refer to it.
     *
     * @param internalFormat The number of color components of the texture, and their size.
     * Refer to the link given above, at the \b internalFormat section
     * for further informations about available internal formats.
     *
     * @param width Width of the 1D texture.
     *
     * @param format The format of the pixel data.
     * Refer to the link given above, at the \b format section
     * for further informations about the available formats.
     *
     * @param type The data type of the pixel data.
     * Refer to the link given above, at the \b type section
     * for further informations about the available types.
     *
     * @param data Data contained in the texture. Can be nullptr. <br/>
     * If \b data is not null, the texture will take the ownership of it.
     *
     * @param linearize (default false) : convert the texture from sRGB to Linear RGB color space
     *
     * @param mipmaped (default false) : generate a prefiltered mipmap for the texture.
     */
    void Generate(uint width, GLenum format, void *data = nullptr, bool linearize = false, bool mipmaped = false);

    /**
     * @brief Init the texture 2D from OpenGL point of view.
     *
     * Generate, bind and configure OpenGL texture.<br/>
     * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are
     * generated.<br/><br/>
     *
     * It is highly recommended to take a look at
     * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage2D">glTexImage2D documentation</a>
     * since this method doc will highly refer to it.
     *
     * @param internalFormat The number of color components of the texture, and their size.
     * Refer to the link given above, at the \b internalFormat section
     * for further informations about available internal formats.
     *
     * @param width Width of the 2D texture.
     *
     * @param height Height of the 2D texture.
     *
     * @param format The format of the pixel data.
     * Refer to the link given above, at the \b format section
     * for further informations about the available formats.
     *
     * @param type The data type of the pixel data.
     * Refer to the link given above, at the \b type section
     * for further informations about the available types.
     *
     * @param data Data contained in the texture. Can be nullptr. <br/>
     * If \b data is not null, the texture will take the ownership of it.
     *
     * @param linearize (default false) : convert the texture from sRGB to Linear RGB color space
     *
     * @param mipmaped (default false) : generate a prefiltered mipmap for the texture.
     */
    void Generate(uint width, uint height, GLenum format, void *data = nullptr, bool linearize = false, bool mipmaped = false);

    /**
     * @brief Init the texture 3D from OpenGL point of view.
     *
     * Generate, bind and configure OpenGL texture.<br/>
     * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are
     * generated.<br/><br/>
     *
     * It is highly recommended to take a look at
     * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage3D">glTexImage3D documentation</a>
     * since this method doc will highly refer to it.
     *
     * @param internalFormat The number of color components of the texture, and their size.
     * Refer to the link given above, at the \b internalFormat section
     * for further informations about available internal formats.
     *
     * @param width Width of the 3D texture.
     *
     * @param height Height of the 3D texture.
     *
     * @param depth Depth of the 3D texture.
     *
     * @param format The format of the pixel data.
     * Refer to the link given above, at the \b format section
     * for further informations about the available formats.
     *
     * @param type The data type of the pixel data.
     * Refer to the link given above, at the \b type section
     * for further informations about the available types.
     *
     * @param data Data contained in the texture. Can be nullptr. <br/>
     * If \b data is not null, the texture will take the ownership of it.
     *
     * @param linearize (default false) : convert the texture from sRGB to Linear RGB color space
     *
     * @param mipmaped (default false) : generate a prefiltered mipmap for the texture.
    */
    void Generate(uint width,
                  uint height,
                  uint depth,
                  GLenum format,
                  void *data = nullptr,
                  bool linearize = false,
                  bool mipmaped = false);

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
     * @param internalFormat The number of color components of the texture, and their size.
     * Refer to the link given above, at the \b internalFormat section
     * for further informations about available internal formats.
     *
     * @param width Width of the six 2D textures.
     *
     * @param height Height of the six 2D textures.
     *
     * @param format The format of the pixel data.
     * Refer to the link given above, at the \b format section
     * for further informations about the available formats.
     *
     * @param type The data type of the pixel data.
     * Refer to the link given above, at the \b type section
     * for further informations about the available types.
     *
     * @param data Data contained in the texture. Can be nullptr. <br/>
     * If \b data is not null, the texture will take the ownership of it.
     *
     * @param linearize (default false) : convert the texture from sRGB to Linear RGB color space
     *
     * @param mipmaped (default false) : generate a prefiltered mipmap for the texture.
     */
    void GenerateCube(uint width, uint height, GLenum format, void **data = nullptr, bool linearize = false, bool mipmaped = false);

    /**
     * @brief Bind the texture to enable its use in a shader
     * @param unit Index of the texture to be bound. If -1 only calls glBindTexture.
     */
    void bind(int unit = -1);

    /**
     * @return Name of the texture.
     */
    inline std::string getName() const { return m_name; }

    /**
     * Update the data contained by the texture
     * @param newData The new data, must contain the same number of elements than old data, no check
     * will be performed.
     */
    void updateData( void* newData );

    /**
     * Update the parameters contained by the texture.
     * User first modify the public attributes corresponding to the parameter he wants to change the value
     * (e.g wrap* or *Filter) and call this function to update the OpenGL texture state ...
     * @return
     */
     void updateParameters();

    /**
    * Convert a color texture from sRGB to Linear RGB spaces.
    * This will transform the internal representation of the texture to GL_FLOAT.
    * Only GL_RGB[8, 16, 16F, 32F] and GL_RGBA[8, 16, 16F, 32F] are managed.
    * Full transformation as described at https://en.wikipedia.org/wiki/SRGB
    * @param gamma the gama value to use (sRGB is 2.4
    */
    void linearize(Scalar gamma = Scalar(2.4));

    GLenum format() const { return m_format; }
    uint width() const { return m_width; }
    uint height() const { return m_height; }
    uint depth() const { return m_depth; }
    globjects::Texture* texture() const { return m_texture.get(); }


  private:

    /**
     * Convert a color texture from sRGB to Linear RGB spaces.
     * This will transform the internal representation of the texture to GL_FLOAT.
     * Only GL_RGB[8, 16, 16F, 32F] and GL_RGBA[8, 16, 16F, 32F] are managed.
     * Full transformation as described at https://en.wikipedia.org/wiki/SRGB
     * @param texels the array of texels to linearize
     * @param numCommponent number of color channels.
     * @param bool hasAlphaChannel indicate if the last channel is an alpha channel.
     * @param gamma the gama value to use (sRGB is 2.4)
     * @note only 8 bit textures are managed by this operator.
    */
    void sRGBToLinearRGB(uint8_t *texels, int numCommponent, bool hasAlphaChannel, Scalar gamma = Scalar(2.4));

private:
    /// name of the texture
    std::string m_name;

    /// OpenGLstate associated with the texture
    GLenum m_target;
    GLenum m_format;

    /// Sizes of the texture
    uint m_width;
    uint m_height;
    uint m_depth;

    /// Link to glObject texture
    std::unique_ptr<globjects::Texture> m_texture;

    /// Is the texture mipmaped ?
    bool m_isMipMaped;
    /// Is the texture in LinearRGB ?
    bool m_isLinear;

    /// Texels of the texture
    void *m_texels;
};
} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTURE_HPP
