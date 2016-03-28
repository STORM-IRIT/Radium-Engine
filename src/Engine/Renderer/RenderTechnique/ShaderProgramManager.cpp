#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <cstdio>

#include <Core/Containers/MakeShared.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>

namespace Ra
{
    namespace Engine
    {
        using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;

        ShaderProgramManager::ShaderProgramManager(const std::string& vs, const std::string& fs)
        {
            m_defaultShaderProgram = addShaderProgram("Default Program", vs, fs);
        }

        ShaderProgramManager::~ShaderProgramManager()
        {
            m_shaderPrograms.clear();
        }

        const ShaderProgram* ShaderProgramManager::addShaderProgram(const std::string& name, const std::string& vert, const std::string& frag)
        {
            ShaderConfiguration config(name);
            config.addShader(ShaderType_VERTEX, vert);
            config.addShader(ShaderType_FRAGMENT, frag);
            return addShaderProgram(config);
        }

        const ShaderProgram* ShaderProgramManager::addShaderProgram(const ShaderConfiguration& config)
        {

            auto found = m_shaderPrograms.find(config);

            if (found != m_shaderPrograms.end())
            {
                return found->second.get();
            }
            else
            {
                // Try to load the shader
                auto prog = Core::make_shared<ShaderProgram>(config);
                if (prog->isOk())
                {
                    insertShader(config, prog);
                    return prog.get();
                }
                else
                {
                    std::string error;
                    Core::StringUtils::stringPrintf( error,
                        "Error occurred while loading shader program %s :\nDefault shader program used instead.\n",
                        config.m_name.c_str() );
                    LOG( logERROR ) << error;
                    return m_defaultShaderProgram;
                }
            }
        }

        const ShaderProgram* ShaderProgramManager::getShaderProgram(const std::string &id)
        {
            auto found = m_shaderProgramIds.find(id);
            if (found != m_shaderProgramIds.end())
            {
                return getShaderProgram(found->second);
            }

            return nullptr;
        }

        const ShaderProgram* ShaderProgramManager::getShaderProgram(const ShaderConfiguration& config)
        {
            return addShaderProgram(config);
        }

        void ShaderProgramManager::reloadAllShaderPrograms()
        {
            // For each shader in the map
            for (auto& shader : m_shaderPrograms)
            {
                shader.second->reload();
            }
        }

        const ShaderProgram* ShaderProgramManager::getDefaultShaderProgram() const
        {
            return m_defaultShaderProgram;
        }

        void ShaderProgramManager::insertShader(const ShaderConfiguration& config, const ShaderProgramPtr& shader)
        {
            m_shaderProgramIds.insert(std::pair<std::string, ShaderConfiguration>(config.m_name, config));
            m_shaderPrograms.insert(std::pair<ShaderConfiguration, ShaderProgramPtr>(config, shader));
        }

        RA_SINGLETON_IMPLEMENTATION( ShaderProgramManager );
    }// namespace Engine
} // namespace Ra
