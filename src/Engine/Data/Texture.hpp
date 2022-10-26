#pragma once

#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Color.hpp>
#include <Engine/OpenGL.hpp>
#include <Engine/RaEngine.hpp>

#include <memory>
#include <mutex>
#include <string>

namespace globjects {
class Texture;
}

namespace Ra {
namespace Engine {
namespace Data {

struct SamplerParameters {
    /// OpenGL wrap mode in the s direction
    GLenum wrapS { GL_CLAMP_TO_EDGE };
    /// OpenGL wrap mode in the t direction
    GLenum wrapT { GL_CLAMP_TO_EDGE };
    /// OpenGL wrap mode in the p direction
    GLenum wrapP { GL_CLAMP_TO_EDGE };
    /// OpenGL minification filter ( GL_LINEAR or GL_NEAREST or GL_XXX_MIPMAP_YYY )
    GLenum minFilter { GL_LINEAR };
    /// OpenGL magnification filter ( GL_LINEAR or GL_NEAREST )
    GLenum magFilter { GL_LINEAR };
};
inline bool operator==( const SamplerParameters& lhs, const SamplerParameters& rhs ) {
    return lhs.wrapS == rhs.wrapS && lhs.wrapT == rhs.wrapT && lhs.wrapP == rhs.wrapP &&
           lhs.minFilter == rhs.minFilter && lhs.magFilter == rhs.magFilter;
}
inline bool operator!=( const SamplerParameters& lhs, const SamplerParameters& rhs ) {
    return !( lhs == rhs );
}

struct ImageParameters {
    /// OpenGL target
    GLenum target { GL_TEXTURE_2D };
    /// width of the texture (s dimension)
    size_t width { 1 };
    /// height of the texture (t dimension)
    size_t height { 1 };
    /// width of the texture (p dimension)
    size_t depth { 1 };
    /// Format of the external data
    GLenum format { GL_RGB };
    /// OpenGL internal format (WARNING, for Integer textures, must be GL_XXX_INTEGER)
    GLenum internalFormat { GL_RGB };
    /// Type of the components in external data
    GLenum type { GL_UNSIGNED_BYTE };

    /// texels OR cubeMap, shared ownership
    std::shared_ptr<void> texels { nullptr };
    std::array<std::shared_ptr<void>, 6> cubeMap {};
};

inline bool operator==( const ImageParameters& lhs, const ImageParameters rhs ) {
    return lhs.target == rhs.target && lhs.width == rhs.width && lhs.height == rhs.height &&
           lhs.format == rhs.format && lhs.internalFormat == rhs.internalFormat &&
           lhs.type == rhs.type && lhs.texels == rhs.texels && lhs.cubeMap == rhs.cubeMap;
}
inline bool operator!=( const ImageParameters& lhs, const ImageParameters& rhs ) {
    return !( lhs == rhs );
}

/** \brief Describes the content and parameters of a texture.
 * This structures encapsulates all the states used for creating an OpenGL texture.
 *  These parameters describe the image data of the texture :
 *    - target, width, height, depth, format, internalFormat, type and texels for describing image
 * data
 *    - wrapS, wrapT, wrapP, minFilter and magFilter for describing the sampler of the texture.
 *
 *  When one wants to create a texture, the first thing to do is to create and fill a Texture
 * parameter structure that will describe the Texture.
 *
 *  The Texture creation could be done either using the TextureManager or directly on the client
 * class/function.
 *
 *  When a texture is created, no OpenGL initialisation is realized. The user must first call
 * initializeGL before being able to use this texture in an OpenGL operation.
 *
 *  MipMap representation of the texture is automatically generated as soon as the minFilter
 * parameter is something else than GL_LINEAR or GL_NEAREST
 *
 * \note No coherence checking will be done on the content of this structure. User must ensure
 * coherent data and parameters before creating the OpenGL texture with Texture::initializeGL
 */
struct TextureParameters {
    std::string name {};
    SamplerParameters sampler {};
    ImageParameters image {};
};

/** \brief Represent a Texture of the engine.
 *
 * See TextureManager for information about how unique texture are defined.
 */
class RA_ENGINE_API Texture final
{
  public:
    /** \brief Textures are not copyable, delete copy constructor.
     */
    Texture( const Texture& ) = delete;

    /** \brief Textures are not copyable, delete operator =.
     */
    void operator=( const Texture& ) = delete;

    /** \brief Texture constructor. No OpenGL initialization is done there.
     *
     * \param texParameters Name of the texture
     */
    explicit Texture( const TextureParameters& texParameters );

    /** \brief Texture destructor. Both internal data and OpenGL stuff are deleted.
     */
    ~Texture();

    /** \brief Generate the OpenGL representation of the texture according to the stored
     * TextureData (delayed).
     *
     * This method use the stored TextureParameters to generate and configure OpenGL
     * texture. It creates gpu tasks the engine will run during next draw call, so it can be called
     * without active opengl context.
     *
     * This method will apply RGB space conversion if \a linearize is true.
     *
     * \param linearize (default false) : convert the texture from sRGB to Linear RGB color space
     * before OpenGL initialisation
     */
    void initialize( bool linearize = false );

    /** \brief Generate the GPU representation of the texture <b>right now</b>. Need an active
     * OpenGL context.
     *
     * see initialze() which is the same method, but delay gpu stuff to engine gpu tasks.
     */
    void initializeNow( bool linearize = false ) {
        if ( !isSupportedTarget() ) return;
        if ( linearize ) { this->linearize(); }
        createTexture();
        computeIsMipMappedFlag();
        sendSamplerParametersToGPU();
        sendImageDataToGPU();
    }

    /// \return Name of the texture.
    inline std::string getName() const { return m_textureParameters.name; }

    /// \return the pixel format of the texture
    GLenum getFormat() const { return m_textureParameters.image.format; }

    /// \return the width of the texture
    size_t getWidth() const { return m_textureParameters.image.width; }

    /// \return the height of the texture
    size_t getHeight() const { return m_textureParameters.image.height; }

    /// \return the depth of the texture
    size_t getDepth() const { return m_textureParameters.image.depth; }

    /// \return raw pointer to texels (or nullptr if no cpu side representation).
    void* getTexels() { return m_textureParameters.image.texels.get(); }

    /** \brief Get the underlying globjects::Texture.
     *
     * Use with care since you can brake the equivalence
     * of image and sampler parameters between cpu Data::Texture and gpu side globlects::Texture.
     * \return the globjects::Texture associated with the texture.
     */
    globjects::Texture* getGPUTexture() const { return m_texture.get(); }

    /// get read access to texture parameters
    const TextureParameters& getParameters() const { return m_textureParameters; }

    /** \brief Update the cpu representation of data contained by the texture.
     *
     * \a newData must contain the same number (of the same type) of elements than old data (not
  checked).
     * Element count can be obtained with getWidth() * getHeight()
     * Element type can be obtained with getFormat()
     * \param newData user image pointer to wrap
     */
    void updateData( std::shared_ptr<void> newData );

    /** \brief Resize the texture. Need active OpenGL context.
     *
     * This allocate GPU memory to store the new resized texture and, if texels are not nullptr,
     * upload the new content.
     * \note : If texels are not nullptr, user must ensure the texels array is correctly
     * dimensioned.
     * \param w width of the texture
     * \param h height of the texture
     * \param d depth of the texture
     * \param pix the new texels array corresponding the the new texture dimension
     */
    void resize( size_t w = 1, size_t h = 1, size_t d = 1, std::shared_ptr<void> pix = nullptr );

    /** \brief set TextureParameters.
     *
     * If imageParameters is changed, the method call setImageParameters() to register update GPU
     * representation task. If samplerParameter is change, the method call setSamplerParameters() to
     * register update GPU sample task.
     */
    void setParameters( const TextureParameters& textureParameters );
    /// \brief set TextureParameters.image
    void setImageParameters( const ImageParameters& imageParameters );
    /// \brief set TerctureParameters.samples
    void setSamplerParameters( const SamplerParameters& samplerParameters );

    /** \brief Bind the texture to GPU texture \a unit to enable its use in a shader. Need active
     * OpenGL context.
     *
     * \param unit Index of the texture to be bound. If -1 (default) only calls glBindTexture.
     */
    void bind( int unit = -1 );

    /** \brief Bind the texture to an image unit for the purpose of reading and writing it from
     * shaders. Need active OpenGL context.
     *
     * \note, only available since openGL 4.2, not available on MacOs
     * uses m_parameters.internalFormat as format.
     * see
     * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml
     * for documentation
     */
    void bindImageTexture( int unit, GLint level, GLboolean layered, GLint layer, GLenum access );

    /** \brief Convert a color texture from sRGB to Linear RGB spaces.
     *
     * This will transform the internal representation of the texture to GL_SCALAR (GL_FLOAT).
     * Only GL_RGB[8, 16, 16F, 32F] and GL_RGBA[8, 16, 16F, 32F] are managed.
     * Full transformation as described at https://en.wikipedia.org/wiki/SRGB
     */
    void linearize();

  private:
    /**
     * Current implementation supports GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE,
     * GL_TEXTURE_3D and GL_TEXTURE_CUBE_MAP.
     * \return true if the target is supported by Texture implementation.
     */
    bool isSupportedTarget();

    /// \brief set m_isMipMapped according to sampler.minFilter
    void computeIsMipMappedFlag();

    /** \brief Allocate m_texture if nullptr.
     *
     * \return true if allocation is actually performed
     */
    bool createTexture();

    /// \brief Regiter gpu task to RadiumEngine
    void registerUpdateImageDataTask();

    /// \brief Regiter gpu task to RadiumEngine
    void registerUpdateSamplerParametersTask();

    /// \brief Send image data to the GPU and generate mipmap if needed
    void sendImageDataToGPU();

    /// \brief Send sampler parameters to the GPU
    void sendSamplerParametersToGPU();

    /** \brief Convert a color texture from sRGB to Linear RGB spaces.
     *
     * The content of the array of texels.
     * designated by the texel pointer is modified by side effect.
     * Full transformation as described at https://en.wikipedia.org/wiki/SRGB
     * \param texels the array of texels to linearize
     * \param numComponent number of color channels.
     * \param bool hasAlphaChannel indicate if the last channel is an alpha channel.
     * \param gamma the gama value to use (sRGB is 2.4)
     * \note only 8 bit (GL_UNSIGNED_BYTE data format) textures are managed by this operator.
     */
    void sRGBToLinearRGB( uint8_t* texels, uint numComponent, bool hasAlphaChannel );

    /// \brief linearize a cube map by calling sRGBToLinearRGB fore each face
    void linearizeCubeMap( uint numComponent, bool hasAlphaChannel );

    /// Texture parameters
    TextureParameters m_textureParameters;

    /// Link to glObject texture
    std::unique_ptr<globjects::Texture> m_texture;
    /// Is the texture mip-mapped ?
    bool m_isMipMapped { false };
    /// Is the texture in LinearRGB ?
    bool m_isLinear { false };
    /// \brief This task is valid when a gpu image update task is registered (e.g. after a call to
    /// initialize, setParameters, setImageParameters or setData).
    Core::TaskQueue::TaskId m_updateImageTaskId;
    /// \brief This task is valid when a gpu sampler task is registered.
    /// e.g. after a call to initialize, setParamaters or setSamplerParamters).
    Core::TaskQueue::TaskId m_updateSamplerTaskId;
    /// mutex to protect non gpu setters, in a thread safe way.
    std::mutex m_updateMutex;
};
} // namespace Data
} // namespace Engine
} // namespace Ra
