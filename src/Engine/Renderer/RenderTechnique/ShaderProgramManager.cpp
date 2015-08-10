#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <cstdio>

#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <Core/String/StringUtils.hpp>

namespace Ra
{

    Engine::ShaderProgramManager::ShaderProgramManager( const std::string& shaderPath,
                                                        const std::string& defaultShaderProgram )
        : m_shaderPath( shaderPath )
    {
        ShaderConfiguration config = getDefaultShaderConfiguration( defaultShaderProgram );
        m_defaultShaderProgram = new ShaderProgram( config );
    }

    Engine::ShaderProgramManager::~ShaderProgramManager()
    {
        m_shaderPrograms.clear();
        m_shaderProgramStatus.clear();
    }

    Engine::ShaderProgram* Engine::ShaderProgramManager::addShaderProgram( const std::string& name )
    {
        ShaderConfiguration config = getDefaultShaderConfiguration( name );
        return addShaderProgram( config );
    }

    Engine::ShaderProgram* Engine::ShaderProgramManager::addShaderProgram( const ShaderConfiguration& config )
    {
        ShaderProgram* ret;

        // Check if not already inserted
        if ( m_shaderPrograms.find( config ) != m_shaderPrograms.end() )
        {
            ret = m_shaderPrograms[config];
        }
        else
        {
            // Try to load the shader
            ShaderProgram* shader = new ShaderProgram( config );
            if ( shader->isOk() )
            {
                insertShader( config, shader, ShaderProgramStatus::COMPILED );
                ret = shader;
            }
            else
            {
                std::string error;
                Core::StringUtils::stringPrintf( error,
                                                 "Error occurred while loading shader program %s :\nDefault shader program used instead.\n",
                                                 config.getName().c_str() );
                CORE_WARN_IF( true, error.c_str() );
                ret = m_defaultShaderProgram;
            }
        }

        return ret;
    }

    Engine::ShaderProgram* Engine::ShaderProgramManager::getShaderProgram( const ShaderConfiguration& config )
    {
        ShaderProgram* ret;

        if ( m_shaderPrograms.find( config ) != m_shaderPrograms.end() )
        {
            // Already in the map
            ret = m_shaderPrograms[config];
        }
        else
        {
            ret = addShaderProgram( config );
        }

        return ret;
    }

    void Engine::ShaderProgramManager::reloadAllShaderPrograms()
    {
        // For each shader in the map
        for ( auto shader : m_shaderPrograms )
        {
            if ( m_shaderProgramStatus.at( shader.first ) == ShaderProgramStatus::COMPILED )
            {
                // Shader program has already been compiled successfully, try to reload it
                shader.second->reload();
                if ( !shader.second->isOk() )
                {
                    std::string error;
                    Core::StringUtils::stringPrintf( error,
                                                     "Error occurred while loading shader program %s :\nDefault shader program used instead.\n",
                                                     shader.first.getName().c_str() );
                    CORE_WARN_IF( true, error.c_str() );
                    m_shaderPrograms.at( shader.first ) = m_defaultShaderProgram;
                    m_shaderProgramStatus.at( shader.first ) = ShaderProgramStatus::NOT_COMPILED;
                }
            }
            else
            {
                ShaderProgram* newShader = new ShaderProgram( shader.first );
                if ( newShader->isOk() )
                {
                    // Ok compiled, register it in the map
                    m_shaderPrograms.at( shader.first ) = newShader;
                    m_shaderProgramStatus.at( shader.first ) = ShaderProgramStatus::COMPILED;
                }
                else
                {
                    std::string error;
                    Core::StringUtils::stringPrintf( error,
                                                     "Error occurred while loading shader program %s :\nDefault shader program used instead.\n",
                                                     shader.first.getName().c_str() );
                    CORE_WARN_IF( true, error.c_str() );
                }
            }
        }
    }

    void Engine::ShaderProgramManager::reloadNotCompiledShaderPrograms()
    {
        for ( auto shader : m_shaderPrograms )
        {
            // Just look not compiled shaders
            if ( m_shaderProgramStatus.at( shader.first ) == ShaderProgramStatus::NOT_COMPILED )
            {

                ShaderProgram* newShader = new ShaderProgram( shader.first );
                if ( newShader->isOk() )
                {
                    // Ok compiled, register it in the map
                    m_shaderPrograms.at( shader.first ) = newShader;
                    m_shaderProgramStatus.at( shader.first ) = ShaderProgramStatus::COMPILED;
                }
                else
                {
                    std::string error;
                    Core::StringUtils::stringPrintf( error,
                                                     "Error occurred while loading shader program %s :\nDefault shader program used instead.\n",
                                                     shader.first.getName().c_str() );
                    CORE_WARN_IF( true, error.c_str() );
                }
            }
        }
    }

    Engine::ShaderConfiguration Engine::ShaderProgramManager::getDefaultShaderConfiguration(
        const std::string& shaderName )
    {
        return ShaderConfiguration( shaderName, m_shaderPath );
    }

    Engine::ShaderProgram* Engine::ShaderProgramManager::getDefaultShaderProgram() const
    {
        return m_defaultShaderProgram;
    }

    std::string Engine::ShaderProgramManager::getFullShaderName( const std::string& shaderName )
    {
        std::stringstream ss;
        ss << m_shaderPath << '/' << shaderName;
        return ss.str();
    }

    void Engine::ShaderProgramManager::insertShader( const ShaderConfiguration& config,
                                                     ShaderProgram* shader,
                                                     const ShaderProgramStatus& status )
    {
        m_shaderPrograms.insert( std::pair<ShaderConfiguration, ShaderProgram*> ( config, shader ) );
        m_shaderProgramStatus.insert( std::pair<ShaderConfiguration, ShaderProgramStatus> ( config, status ) );
    }

} // namespace Ra
