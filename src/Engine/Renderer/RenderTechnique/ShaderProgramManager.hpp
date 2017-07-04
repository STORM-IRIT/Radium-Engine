#ifndef RADIUMENGINE_SHADERPROGRAMANAGER_HPP
#define RADIUMENGINE_SHADERPROGRAMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <Core/Utils/Singleton.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

namespace globjects
{
    class File;
    class NamedString;
}

namespace Ra
{
    namespace Engine
    {
        class ShaderProgram;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API ShaderProgramManager
        {
            RA_SINGLETON_INTERFACE(ShaderProgramManager);

        public:
            const ShaderProgram* addShaderProgram(const std::string& name, const std::string& vert, const std::string& frag);
            const ShaderProgram* addShaderProgram(const ShaderConfiguration& config);

            const ShaderProgram* getShaderProgram(const std::string& id);
            const ShaderProgram* getShaderProgram(const ShaderConfiguration& config);

            const ShaderProgram* getDefaultShaderProgram() const;

            void reloadAllShaderPrograms();
            void reloadNotCompiledShaderPrograms();

        private:
            ShaderProgramManager(const std::string& vs, const std::string& fs);
            ~ShaderProgramManager();

            void initialize();
            void insertShader(const ShaderConfiguration& config, const std::shared_ptr<ShaderProgram>& shader);

        private:
            std::map<std::string, ShaderConfiguration> m_shaderProgramIds;
            std::map<ShaderConfiguration, std::shared_ptr<ShaderProgram>> m_shaderPrograms;
            std::vector<ShaderConfiguration> m_shaderFailedConfs;

            std::vector<std::unique_ptr<globjects::File>> m_files;
            std::vector<std::unique_ptr<globjects::NamedString>> m_namedStrings;

            const ShaderProgram* m_defaultShaderProgram;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERMANAGER_HPP
