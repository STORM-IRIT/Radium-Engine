#ifndef RADIUMENGINE_SHADERCONFIGURATION_HPP
#define RADIUMENGINE_SHADERCONFIGURATION_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <list>
#include <set>
#include <string>
#include <vector>

namespace Ra {
namespace Engine {
/**
 * Available shader types.
 */
enum ShaderType : uint {
    ShaderType_VERTEX = 0,
    ShaderType_FRAGMENT,
    ShaderType_GEOMETRY,
    ShaderType_TESS_CONTROL,
    ShaderType_TESS_EVALUATION,
    ShaderType_COMPUTE,
    ShaderType_COUNT
};

// clag-format off
/**
 * A class used to create shader programs later on.
 * A ShaderConfiguration should be added once to the ShaderConfigurationFactory,
 * then the factory must be used to retrieve the added shader configurations.
 *
 * Typical use case :
 * \code
 *      **************************** CREATION ****************************
 *     // Create the shader configuration once (see MainApplication::addBasicShaders for example).
 *     ShaderConfiguration config("MyConfig");
 *     // Add shader files to the config (note that the file extensions can be whatever you want).
 *     config.addShader(ShaderType_VERTEX, "path/to/shader.vert.glsl");
 *     config.addShader(ShaderType_FRAGMENT, "path/to/shader.frag.glsl");
 *     // Same for other shader types. Vertex and fragment are required, other are optional.
 *     // Note that, for a compute shader, only the compute shader is needed.
 *     // Add eventually some properties to the shader configuration.
 *     config.addProperties( {"USE_MICROFACET;", "MICROFACET_DISTRIBUTION Trowbridge_Reitz;"} );
 *     // Add also some includes if needed.
 *     config.addInclude{"\"MicrofacetFunctions.glsl\";");
 *     // Add the configuration to the factory.
 *     ShaderConfigurationFactory::addConfiguration(config);
 *
 *      **************************** USAGE ****************************
 *     // When you want to reuse created shader configurations, just set it using.
 *     auto config = ShaderConfigurationFactory::getConfiguration("MyConfig");
 *     // You can then use it to extract the shader from the ShaderProgramManager and use it.
 *     auto shader = ShaderProgramManager::getInstance()->getShaderProgram();
 *     shader->bind();
 *     ...
 * \endcode
 *
 * \todo Make configuration and program (in the OpenGL sense) be packed.
 *       A shader configuration might contains a ShaderProgram after compiling.
 *       This will remove the need for ShaderConfigurationFactory as
 *       ShaderManager will offer the same services.
 *       Actually, there is redundancy between ShaderConfigurationFactory
 *       and ShaderProgramManager
 */
// clag-format on
class RA_ENGINE_API ShaderConfiguration final {
    friend class ShaderProgram;

  public:
    ShaderConfiguration() = default;
    /**
     * Initializes a ShaderConfiguration with the given name.
     * \warning This does not query the corresponding configuration in the
     *          ShaderConfigurationFactory. The proper way to do this is by
     *          calling ShaderConfigurationFactory::getConfiguration(\p name).
     */
    explicit ShaderConfiguration( const std::string& name );

    /**
     * Initializes a ShaderConfiguration with a name, a vertex and a fragment shader.
     * \note This does not add the configuration to the factory,
     *       ShaderConfigurationFactory::addConfiguration() must be called.
     */
    ShaderConfiguration( const std::string& name, const std::string& vertexShader,
                         const std::string& fragmentShader );

    /**
     * Add a shader given its type.
     */
    void addShader( ShaderType type, const std::string& name );

    /**
     * Add a property in the form of a \#define.
     * \note Properties are set for each shader.
     * \note The same shader file with different properties leads to different
     *       shader programs.
     */
    void addProperty( const std::string& prop );

    /**
     * Add each property in the form of a \#define.
     * \note Properties are set for each shader.
     * \note The same shader file with different properties leads to different
     *       shader programs.
     */
    void addProperties( const std::list<std::string>& props );

    /**
     * Remove the given property.
     * \note The same shader files with different properties leads to different
     *       shader programs.
     */
    void removeProperty( const std::string& prop );

    /**
     * Return the list of \#define properties.
     */
    std::set<std::string> getProperties() const;

    /**
     * Add a property in the form of a \#include in all shaders of the given type.
     * \note Properties order might be crucial for the shaders to compile.
     * \note The same shader files with different properties leads to different
     *       shader programs.
     */
    void addInclude( const std::string& incl, ShaderType type = ShaderType_FRAGMENT );

    /**
     * Add each property in the form of a \#include in all shaders of the given type.
     * \note The properties order might be crucial for the shaders to compile.
     * \note The same shader files with different properties leads to different
     *       shader programs.
     */
    void addIncludes( const std::list<std::string>& incls, ShaderType type = ShaderType_FRAGMENT );

    /**
     * Remove the given property for the given shader type.
     * \note The properties order might be crucial for the shaders to compile.
     * \note The same shader files with different properties leads to different
     *       shader programs.
     */
    void removeInclude( const std::string& incl, ShaderType type = ShaderType_FRAGMENT );

    /**
     * Return the list of \#include properties, along with the shader type using them.
     */
    const std::vector<std::pair<std::string, ShaderType>>& getIncludes() const;

    /**
     * Tell if a shader configuration has at least a vertex and a fragment shader,
     * or a compute shader.
     * */
    bool isComplete() const;

    /**
     * Return true if (checked in that order):
     *  - the first shader name in *this, which is different from its type-wise
     *    correspondant in \p other, lexically compares lower to it;
     *  - shader names are the same but *this has less \#define properties;
     *  - shader names are the same, the number of \#define properties is the same,
     *    but the first \#define property in *this, which is different from its
     *    order-wise correspondant in \p other, lexically compares lower to it;
     *  - shader names are the same, there are no \#define properties,
     *    the number of \#include properties is the same,
     *    but the first \#include property in *this, which is different from its
     *    order-wise correspondant in \p other, lexically compares lower to it;
     *
     * false otherwise.
     */
    // FIXME: what if same "define" properties but different include ones?
    // FIXME: what if same number of "include" properties is different?
    bool operator<( const ShaderConfiguration& other ) const;

    /**
     * Return default shader configuration.
     */
    static ShaderConfiguration getDefaultShaderConfig() { return m_defaultShaderConfig; }

  public:
    /**
     * The name of the ShaderConfiguration.
     */
    std::string m_name{};

    /**
     * The GLSL version to consider for the Shaders.
     */
    std::string m_version{};

  private:
    /**
     * The list of shaders' name.
     */
    std::array<std::string, ShaderType_COUNT> m_shaders;

    /**
     * The list of \#define properties.
     */
    std::set<std::string> m_properties;

    /**
     * The list of \#include properties, along with the shader type using them.
     */
    std::vector<std::pair<std::string, ShaderType>> m_includes;

    /**
     * The default shader configuration.
     */
    static ShaderConfiguration m_defaultShaderConfig;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERCONFIGURATION_HPP
