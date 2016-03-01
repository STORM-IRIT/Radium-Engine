#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <cstdio>

#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>

namespace Ra
{
namespace Engine 
{

    ShaderProgramManager::ShaderProgramManager( const std::string& shaderPath,
        const std::string& defaultShaderProgram )
        : m_shaderPath( shaderPath )
    {
        ShaderConfiguration config = getDefaultShaderConfiguration( defaultShaderProgram );
        m_defaultShaderProgram = new ShaderProgram( config );
    }

    ShaderProgramManager::~ShaderProgramManager()
    {
        m_shaderPrograms.clear();
        m_shaderProgramStatus.clear();
    }

    ShaderProgram* ShaderProgramManager::addShaderProgram( const std::string& name )
    {
        ShaderConfiguration config = getDefaultShaderConfiguration( name );
        return addShaderProgram( config );
    }

    ShaderProgram* ShaderProgramManager::addShaderProgram( const ShaderConfiguration& config )
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
                LOG( logERROR ) << error;
                ret = m_defaultShaderProgram;
            }
        }

        return ret;
    }

    ShaderProgram* ShaderProgramManager::getShaderProgram( const ShaderConfiguration& config )
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

    void ShaderProgramManager::reloadAllShaderPrograms()
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

    void ShaderProgramManager::reloadNotCompiledShaderPrograms()
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

    ShaderConfiguration ShaderProgramManager::getDefaultShaderConfiguration(
        const std::string& shaderName )
    {
        return ShaderConfiguration( shaderName, m_shaderPath );
    }

    ShaderProgram* ShaderProgramManager::getDefaultShaderProgram() const
    {
        return m_defaultShaderProgram;
    }

    std::string ShaderProgramManager::getFullShaderName( const std::string& shaderName )
    {
        std::stringstream ss;
        ss << m_shaderPath << '/' << shaderName;
        return ss.str();
    }

    void ShaderProgramManager::insertShader( const ShaderConfiguration& config,
        ShaderProgram* shader,
        const ShaderProgramStatus& status )
    {
        m_shaderPrograms.insert( std::pair<ShaderConfiguration, ShaderProgram*>( config, shader ) );
        m_shaderProgramStatus.insert( std::pair<ShaderConfiguration, ShaderProgramStatus>( config, status ) );
    }

    RA_SINGLETON_IMPLEMENTATION( ShaderProgramManager );
}// namespace Engine
} // namespace Ra
