#ifndef RADIUMENGINE_SHADERPROGRAMANAGER_HPP
#define RADIUMENGINE_SHADERPROGRAMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Core/Utils/Singleton.hpp>

namespace globjects {
class File;
class NamedString;
} // namespace globjects

namespace Ra {
namespace Engine {
class ShaderProgram;
class ShaderConfiguration;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

// clang-format off
/**
 * Manage the set of shaders used by Radium Engine for rendering.
 *
 * This class allows to manage ShaderPrograms according to their configuration
 * so that, for a given configuration, there is only one ShaderProgram instance
 * in the System.
 *
 * This manager is a singleton. At the creation of the singleton, one can give
 * parameters that will define the default ShaderProgram.
 * For example :
 * \code
 *     Engine::ShaderProgramManager::createInstance( "Shaders/Default.vert.glsl", "Shaders/Default.frag.glsl" );
 * \endcode
 */
// clang-format on
// FIXME: should go in Engine/Managers
class RA_ENGINE_API ShaderProgramManager final {
    RA_SINGLETON_INTERFACE( ShaderProgramManager );

  public:
    /**
     * Add a ShaderProgram to the program collection according to the given configuration.
     * This method must be called only once an opeGL context is bound.
     * The shader sources corresponding to the configuration will be compiled, linked and verified.
     *
     * \param config the configuration of the program to add to the collection.
     * \return the created ShaderProgram. In case of compile/link/verify error,
     *         the default ShaderProgram is returned.
     * \note Ownership on the returned pointer is kept by the manager.
     * \bug There is no way to know if the returned program is the default one
     *      (except segfault sometimes ...).
     */
    const ShaderProgram* addShaderProgram( const ShaderConfiguration& config );

    /**
     * Return the ShaderProgram corresponding to the given id.
     * \param id Name of the program to retrieve.
     * \return the ShaderProgram retrieved, nullptr if the program doesn't exist.
     */
    const ShaderProgram* getShaderProgram( const std::string& id );

    /**
     * Return the ShaderProgram corresponding to the given configuration.
     * \param config Name of the program to retrieve.
     * \return the ShaderProgram retrieved.
     * \note If no ShaderProgram corresponding to the configuration could be found,
     *       returns the result of addShaderProgram(config).
     */
    const ShaderProgram* getShaderProgram( const ShaderConfiguration& config );

    /**
     * Return the default ShaderProgram.
     * Default programs can be defined when building the singleton of the ShaderProgramManager.
     */
    const ShaderProgram* getDefaultShaderProgram() const;

    /**
     * Reload source, recompile, link and validate all the managed programs.
     * \note Usefull for shaders development.
     */
    void reloadAllShaderPrograms();

    /**
     * Programs that did not compiled are temporarilly stored and could be
     * reloaded and compiled when one call this method.
     * If the reloaded program is ok, it is removed from the set of not
     * compiled programs and added to the program collection.
     */
    // FIXME: shouldn't this be private?
    void reloadNotCompiledShaderPrograms();

    /**
     * Allows to define named strings so that the inclusion mechanism of OpenGL could be efficient.
     * A name string associates the name that will be used by a \#define directive
     * in a glsl source file with a file that contains the included glsl source code.
     */
    // FIXME: shouldn't this be private?
    void addNamedString( const std::string& includepath, const std::string& realfile );

    /**
     * Reload all the registered named string.
     * \note Usefull for shader development.
     */
    // FIXME: shouldn't this be private?
    void reloadNamedString();

  private:
    /**
     * Create a ShaderProgramManager whose default ShaderProgram consists in
     * the given ShaderType_VERTEX and ShaderType_FRAGMENT shader files.
     * \note Needs initialization after ctr and before use.
     */
    ShaderProgramManager( const std::string& vs, const std::string& fs );

    ~ShaderProgramManager();

    /**
     * Initialize the include path for some shader files and creates the default ShaderProgram.
     */
    void initialize();

    /**
     * Add the given ShaderProgram to the storage.
     */
    void insertShader( const ShaderConfiguration& config,
                       const std::shared_ptr<ShaderProgram>& shader );

  private:
    /// The list of ShaderConfigurations, unique by name.
    std::map<std::string, ShaderConfiguration> m_shaderProgramIds;

    /// The list of ShaderProgram, unique by configuration.
    std::map<ShaderConfiguration, std::shared_ptr<ShaderProgram>> m_shaderPrograms;

    /// The list of ShaderConfigurations for which the load failed.
    std::vector<ShaderConfiguration> m_shaderFailedConfs;

    /// The list of shader files with additional include path.
    std::vector<std::unique_ptr<globjects::File>> m_files;

    /// The list of additional include path for each shader file needing one.
    std::vector<std::unique_ptr<globjects::NamedString>> m_namedStrings;

    /// The default ShaderType_VERTEX ShaderProgram file.
    std::string m_defaultVsName{};

    /// The default ShaderType_FRAGMENT ShaderProgram file.
    std::string m_defaultFsName{};

    /// The default ShaderProgram.
    const ShaderProgram* m_defaultShaderProgram{nullptr};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERMANAGER_HPP
