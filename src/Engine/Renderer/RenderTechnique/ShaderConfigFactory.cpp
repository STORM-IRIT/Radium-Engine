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

Core::Utils::optional<ShaderConfiguration> getConfiguration( const std::string& name ) {
    if ( name.empty() )
    {
        LOG( logWARNING ) << "Empty name in ShaderConfigurationFactory::getConfiguration call.";
        return {};
    }

    auto found = configs.find( name );
    if ( found != configs.end() ) { return found->second; }
    else
    { return {}; }
}

} // namespace ShaderConfigurationFactory
} // namespace Engine
} // namespace Ra
