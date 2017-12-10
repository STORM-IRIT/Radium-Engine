#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <globjects/base/File.h>

#include <globjects/Shader.h>
#include <globjects/Program.h>
#include <globjects/NamedString.h>

#include <Core/Containers/MakeShared.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

namespace Ra
{
    namespace Engine
    {
        using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;

        ShaderProgramManager::ShaderProgramManager(const std::string& vs, const std::string& fs):
            m_defaultVsName(vs),
            m_defaultFsName(fs)
        {
            initialize();
        }

        ShaderProgramManager::~ShaderProgramManager()
        {
            m_shaderPrograms.clear();
        }

        void ShaderProgramManager::initialize()
        {
            // Create named strings which correspond to shader files that you want to use in shaders's includes.
            // NOTE: if you want to add a named string to handle a new shader include file, be SURE that the name (first
            // parameter) begin with a "/", otherwise it won't work !
            
            m_files.push_back( globjects::File::create( "Shaders/Helpers.glsl" ) );
            m_files.push_back( globjects::File::create( "Shaders/Structs.glsl" ) );
            m_files.push_back( globjects::File::create( "Shaders/Tonemap.glsl" ) );
            m_files.push_back( globjects::File::create( "Shaders/LightingFunctions.glsl" ) );

            m_namedStrings.push_back( globjects::NamedString::create( "/Helpers.glsl", m_files[0].get() ) );
            m_namedStrings.push_back( globjects::NamedString::create( "/Structs.glsl", m_files[1].get() ) );
            m_namedStrings.push_back( globjects::NamedString::create( "/Tonemap.glsl", m_files[2].get() ) );
            m_namedStrings.push_back( globjects::NamedString::create( "/LightingFunctions.glsl", m_files[3].get() ) );

            m_defaultShaderProgram = addShaderProgram("Default Program", m_defaultVsName, m_defaultFsName);

        }

        const ShaderProgram* ShaderProgramManager::addShaderProgram(const std::string& name,
                                                                    const std::string& vert,
                                                                    const std::string& frag)
        {
            ShaderConfiguration config(name);

            config.addShader(ShaderType_VERTEX, vert);
            config.addShader(ShaderType_FRAGMENT, frag);

            return addShaderProgram(config);
        }

        const ShaderProgram* ShaderProgramManager::addShaderProgram(const ShaderConfiguration& config)
        {
            auto found = m_shaderPrograms.find( config );

            if ( found != m_shaderPrograms.end() )
            {
                return found->second.get();
            }

            // Try to load the shader
            auto prog = Core::make_shared<ShaderProgram>( config );

            // FIXED : use isLinked not isValid
            if ( prog->getProgramObject()->isLinked() )
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

                // insert in the failed shaders list
                m_shaderFailedConfs.push_back(config);

                return m_defaultShaderProgram;
            }

        }

        const ShaderProgram* ShaderProgramManager::getShaderProgram(const std::string &id)
        {
            auto found = m_shaderProgramIds.find(id);

            if (found != m_shaderProgramIds.end())
            {
                return getShaderProgram(found->second);
            }

            return m_defaultShaderProgram;
        }

        const ShaderProgram* ShaderProgramManager::getShaderProgram(const ShaderConfiguration& config)
        {
            return addShaderProgram(config);
        }

        void ShaderProgramManager::reloadAllShaderPrograms()
        {
            // For each shader in the map
            for ( auto& shader : m_shaderPrograms )
            {
                shader.second->reload();
            }

            // and also try the failed ones
            reloadNotCompiledShaderPrograms();
        }

        void ShaderProgramManager::reloadNotCompiledShaderPrograms()
        {
            // for each shader in the failed map, try to reload
            for (std::vector<ShaderConfiguration>::iterator conf = m_shaderFailedConfs.begin();
                 conf != m_shaderFailedConfs.end();
                 ++ conf)
            {
                auto prog = Core::make_shared<ShaderProgram>(*conf);

                if (prog->getProgramObject()->isValid())
                {
                    insertShader(*conf, prog);
                    // m_shaderFailedConfs.erase(conf);
                }
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
