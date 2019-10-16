#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <map>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Engine {
namespace ShaderConfigurationFactory {

using namespace Core::Utils; // log

static std::map<std::string, ShaderConfiguration> configs;

void addConfiguration( const ShaderConfiguration& config ) {
    if ( config.m_name.empty() )
    {
        LOG( logWARNING ) << "Empty name in ShaderConfigurationFactory::addConfiguration call. "
                             "Configuration not added";
        return;
    }

    auto found = configs.insert( {config.m_name, config} );
    if ( !found.second )
    {
        LOG( logDEBUG ) << "Configuration " << config.m_name
                        << " already in ShaderConfigurationFactory. "
                             "Configuration not added";
        return;
    }
}

ShaderConfiguration getConfiguration( const std::string& name ) {
    if ( name.empty() )
    {
        LOG( logWARNING ) << "Empty name in ShaderConfigurationFactory::getConfiguration call.";
        return ShaderConfiguration();
    }

    auto found = configs.find( name );
    if ( found != configs.end() ) { return found->second; }
    else
    {
        // Instead of creating a inconsistant configuration, warn and return a default one
        // default configuration is defined as a static member of ShaderConfiguration
        LOG( logWARNING ) << "ShaderConfiguration \"" << name
                          << "\" has not been registered. Return default (added to the factory).";
        ShaderConfiguration config = ShaderConfiguration::getDefaultShaderConfig();
        configs.insert( std::make_pair( config.m_name, config ) );
        return config;
    }
}

} // namespace ShaderConfigurationFactory
} // namespace Engine
} // namespace Ra
