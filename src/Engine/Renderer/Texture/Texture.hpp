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
class RA_ENGINE_API Texture final {
  public:
    GLenum internalFormat = GL_RGB;
    GLenum dataType = GL_UNSIGNED_BYTE;
    GLenum wrapS = GL_REPEAT;
    GLenum wrapT = GL_REPEAT;
    GLenum wrapR = GL_REPEAT;
    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLenum magFilter = GL_LINEAR;

    /**
     * Texture constructor. No OpenGL initialization is done there.
     *
     * @param name Name of the texture
     *
     */
    explicit Texture( std::string name = "" );

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
     */
    void Generate( uint width, GLenum format, void* data = nullptr );

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
     */
    void Generate( uint width, uint height, GLenum format, void* data = nullptr );

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
     */
    void Generate( uint width, uint height, uint depth, GLenum format, void* data = nullptr );

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
     */
    void GenerateCube( uint width, uint height, GLenum format, void** data = nullptr );

    /**
     * @brief Bind the texture to enable its use in a shader
     * @param unit Index of the texture to be bound. If -1 only calls glBindTexture.
     */
    void bind( int unit = -1 );

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

    GLenum format() const { return m_format; }
    uint width() const { return m_width; }
    uint height() const { return m_height; }
    globjects::Texture* texture() const { return m_texture.get(); }

  private:
    Texture( const Texture& ) = delete;
    void operator=( const Texture& ) = delete;

  private:
    GLenum m_target;
    std::string m_name;
    GLenum m_format;

    uint m_width;
    uint m_height;
    uint m_depth;

    std::unique_ptr<globjects::Texture> m_texture;
};
} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTURE_HPP
