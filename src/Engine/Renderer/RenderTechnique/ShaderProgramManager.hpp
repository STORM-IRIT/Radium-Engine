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
}
} // namespace Ra

namespace Ra {
namespace Engine {

class RA_ENGINE_API ShaderProgramManager final {
    RA_SINGLETON_INTERFACE( ShaderProgramManager );

  public:
    const ShaderProgram* addShaderProgram( const std::string& name, const std::string& vert,
                                           const std::string& frag );
    const ShaderProgram* addShaderProgram( const ShaderConfiguration& config );

    const ShaderProgram* getShaderProgram( const std::string& id );
    const ShaderProgram* getShaderProgram( const ShaderConfiguration& config );

    const ShaderProgram* getDefaultShaderProgram() const;

    void reloadAllShaderPrograms();
    void reloadNotCompiledShaderPrograms();

    void addNamedString( const std::string& includepath, const std::string& realfile );
    void updateNamedString();

  private:
    /// need Initialization after ctr and before use
    ShaderProgramManager( const std::string& vs, const std::string& fs );
    ~ShaderProgramManager();
    void initialize();
    void insertShader( const ShaderConfiguration& config,
                       const std::shared_ptr<ShaderProgram>& shader );

  private:
    std::map<std::string, ShaderConfiguration> m_shaderProgramIds;
    std::map<ShaderConfiguration, std::shared_ptr<ShaderProgram>> m_shaderPrograms;
    std::vector<ShaderConfiguration> m_shaderFailedConfs;

    std::vector<std::unique_ptr<globjects::File>> m_files;
    std::vector<std::unique_ptr<globjects::NamedString>> m_namedStrings;

    std::string m_defaultVsName;
    std::string m_defaultFsName;

    const ShaderProgram* m_defaultShaderProgram;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERMANAGER_HPP
