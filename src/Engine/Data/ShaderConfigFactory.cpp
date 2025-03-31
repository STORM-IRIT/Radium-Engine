#include <Core/Utils/Log.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderConfiguration.hpp>
#include <map>
#include <ostream>
#include <utility>

namespace Ra {
namespace Engine {
namespace Data {
namespace ShaderConfigurationFactory {

using namespace Core::Utils; // log

static std::map<std::string, ShaderConfiguration> configs;

void addConfiguration( const ShaderConfiguration& config ) {
    if ( config.getName().empty() ) {
        LOG( logWARNING ) << "Empty name in ShaderConfigurationFactory::addConfiguration call. "
                             "Configuration not added";
        return;
    }

    auto found = configs.insert( { config.getName(), config } );
    if ( !found.second ) {
        LOG( logDEBUG ) << "Configuration " << config.getName()
                        << " already in ShaderConfigurationFactory. "
                           "Configuration not added";
        return;
    }
}

bool removeConfiguration( const std::string& configName ) {
    auto found = configs.find( configName );
    if ( found != configs.end() ) {
        configs.erase( found );
        return true;
    }
    return false;
}

Core::Utils::optional<ShaderConfiguration> getConfiguration( const std::string& name ) {
    if ( name.empty() ) {
        LOG( logWARNING ) << "Empty name in ShaderConfigurationFactory::getConfiguration call.";
        return {};
    }

    auto found = configs.find( name );
    if ( found != configs.end() ) { return found->second; }
    else { return {}; }
}

} // namespace ShaderConfigurationFactory
} // namespace Data
} // namespace Engine
} // namespace Ra
