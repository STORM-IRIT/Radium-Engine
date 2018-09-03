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

/// The ShaderProgramManager class is responsible for storing all the ShaderPrograms
/// used by an Engine.
class RA_ENGINE_API ShaderProgramManager final {
    RA_SINGLETON_INTERFACE( ShaderProgramManager );

  public:
    /// Create a ShaderProgram with the given name and ShaderType_VERTEX and
    /// ShaderType_FRAGMENT shader filenames, if such a program doesn't exist
    /// already, and returns it.
    /// \note If the ShaderProgram cannot be properly loaded, the default
    /// ShaderProgram is returned instead.
    const ShaderProgram* addShaderProgram( const std::string& name, const std::string& vert,
                                           const std::string& frag );

    /// Create a ShaderProgram from the given ShaderConfiguration, if such a
    /// program doesn't exist already, and returns it.
    /// \note If the ShaderProgram cannot be properly loaded, the default
    /// ShaderProgram is returned instead.
    const ShaderProgram* addShaderProgram( const ShaderConfiguration& config );

    /// Return the ShaderProgram whose name matches \p id if it exists,
    /// return the default ShaderProgram otherwise.
    const ShaderProgram* getShaderProgram( const std::string& id );

    /// Return the ShaderProgram with the given ShaderConfiguration if it exists,
    /// return the default ShaderProgram otherwise.
    const ShaderProgram* getShaderProgram( const ShaderConfiguration& config );

    /// Return the default ShaderProgram.
    const ShaderProgram* getDefaultShaderProgram() const;

    /// Reload all the ShaderPrograms. \see ShaderProgram::reload().
    void reloadAllShaderPrograms();

    /// Try to reload only the ShaderPrograms that couldn't be loaded beforehand.
    void reloadNotCompiledShaderPrograms();

    /// Add the given include path for searching the file with name \p realfile.
    // FIXME: shouldn't this be private?
    void addNamedString( const std::string& includepath, const std::string& realfile );

    /// Update the list of additional include paths.
    // FIXME: shouldn't this be private?
    void updateNamedString();

  private:
    /// Create a ShaderProgramManager whose default ShaderProgram consists in
    /// the given ShaderType_VERTEX and ShaderType_FRAGMENT shader files.
    /// \note Need initialization after ctr and before use.
    ShaderProgramManager( const std::string& vs, const std::string& fs );

    ~ShaderProgramManager();

    /// Initialize the include path for some shader files and creates the default ShaderProgram.
    void initialize();

    /// Add the given ShaderProgram to the storage.
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

    /// The default ShaderProgram ShaderType_VERTEX shader file.
    std::string m_defaultVsName;

    /// The default ShaderProgram ShaderType_FRAGMENT shader file.
    std::string m_defaultFsName;

    /// The default ShaderProgram.
    const ShaderProgram* m_defaultShaderProgram;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERMANAGER_HPP
