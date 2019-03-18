#ifndef RADIUMENGINE_SHADERPROGRAM_HPP
#define RADIUMENGINE_SHADERPROGRAM_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/Types.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

#include <array>
#include <memory>
#include <string>

namespace globjects {
class Shader;
class Program;
class NamedString;
} // namespace globjects

namespace Ra {
namespace Engine {
class Texture;

/**
 * Abstraction of OpenGL Shader Program.
 * @see globjects::Program and https://www.khronos.org/opengl/wiki/Shader
 */
class RA_ENGINE_API ShaderProgram final {
  public:
    ShaderProgram();

    explicit ShaderProgram( const ShaderConfiguration& shaderConfig );

    ~ShaderProgram();

    /**
     * Create the program from the given configuration and load the shaders.
     */
    void load( const ShaderConfiguration& shaderConfig );

    /**
     * Reload the shaders.
     */
    void reload();

    /**
     * Return the ShaderConfiguration but without the additional properties.
     */
    ShaderConfiguration getBasicConfiguration() const;

    /**
     * Activates the program.
     */
    void bind() const;

    /**
     * Check the program is valid. If not, prints stat info to the Debug output.
     */
    void validate() const;

    /**
     * Desactivates the program.
     */
    void unbind() const;

    /** \name Uniform setters
     * \see globjects::Program for more information.
     */
    /// \{
    void setUniform( const char* name, int value ) const;

    void setUniform( const char* name, uint value ) const;

    void setUniform( const char* name, float value ) const;

    void setUniform( const char* name, double value ) const;

    void setUniform( const char* name, std::vector<int> value ) const;

    void setUniform( const char* name, std::vector<uint> value ) const;

    void setUniform( const char* name, std::vector<float> value ) const;

    void setUniform( const char* name, const Core::Vector2f& value ) const;

    void setUniform( const char* name, const Core::Vector2d& value ) const;

    void setUniform( const char* name, const Core::Vector3f& value ) const;

    void setUniform( const char* name, const Core::Vector3d& value ) const;

    void setUniform( const char* name, const Core::Vector4f& value ) const;

    void setUniform( const char* name, const Core::Vector4d& value ) const;

    void setUniform( const char* name, const Core::Matrix2f& value ) const;

    void setUniform( const char* name, const Core::Matrix2d& value ) const;

    void setUniform( const char* name, const Core::Matrix3f& value ) const;

    void setUniform( const char* name, const Core::Matrix3d& value ) const;

    void setUniform( const char* name, const Core::Matrix4f& value ) const;

    void setUniform( const char* name, const Core::Matrix4d& value ) const;

    void setUniform( const char* name, Texture* tex, int texUnit ) const;
    /// \}

    /**
     * Bind \p tex on an "arbitrary" texture unit using automatic texture unit computation.
     * If you want to send a particular texture unit, use setUniform().
     * \note Calls std::map::find (in O(log(\# of active texture unit in the shader))).
     */
    void setUniformTexture( const char* name, Texture* tex ) const;

    /**
     * Return the globjects Program.
     */
    globjects::Program* getProgramObject() const;

  private:
    /**
     * Stores the mapping betwween texture unit and bind location.
     */
    struct TextureBinding {
        int m_texUnit{-1};
        int m_location{-1};
        TextureBinding( int unit, int location ) : m_texUnit{unit}, m_location{location} {}
        TextureBinding() = default;
    };

    /**
     * Map from texture name to texture binding.
     */
    using TextureUnits = std::map<std::string, TextureBinding>;

    /**
     * Load and compile a Shader given its type, file name, properties and version.
     */
    void loadShader( ShaderType type, const std::string& name, const std::set<std::string>& props,
                     const std::vector<std::pair<std::string, ShaderType>>& includes,
                     const std::string& version = "#version 410" );

    /**
     * Convert the given ShaderType to its equivalent GLenum.
     */
    GLenum getTypeAsGLEnum( ShaderType type ) const;

    /**
     * Convert the given GLenum to its equivalent ShaderType, or to
     * ShaderType_COUNT if it doesn't account for a shader type.
     */
    ShaderType getGLenumAsType( GLenum type ) const;

    /**
     * Link the program shaders.
     */
    void link();

    /**
     * Process the file inclusion from the \#include properties.
     */
    std::string preprocessIncludes( const std::string& name, const std::string& shader, int level,
                                    int line = 0 );

  private:
    /// Map from texture name to texture binding.
    TextureUnits textureUnits;

    /// The ShaderConfiguration.
    ShaderConfiguration m_configuration;

    /// The list of Shaders, one per stage.
    std::array<std::unique_ptr<globjects::Shader>, ShaderType_COUNT> m_shaderObjects;

    /// The globjects program.
    std::unique_ptr<globjects::Program> m_program;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERPROGRAM_HPP
