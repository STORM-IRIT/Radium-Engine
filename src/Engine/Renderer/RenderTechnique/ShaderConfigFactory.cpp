#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <map>

#include <Core/Log/Log.hpp>

namespace Ra
{
namespace Engine
{
namespace ShaderConfigurationFactory
{

static std::map<std::string, ShaderConfiguration> configs;

void addConfiguration(const ShaderConfiguration &config)
{
    addConfiguration(config.m_name, config);
}

void addConfiguration(const std::string &name, const ShaderConfiguration &config)
{
    if (name.empty())
    {
        LOG(logWARNING) << "Empty name in ShaderConfigurationFactory::addConfiguration call.";
        return;
    }

    configs.insert(std::make_pair(name, config));
}

ShaderConfiguration getConfiguration(const std::string &name)
{
    if (name.empty())
    {
        LOG(logWARNING) << "Empty name in ShaderConfigurationFactory::getConfiguration call.";
        return ShaderConfiguration();
    }

    auto found = configs.find(name);
    if (found != configs.end())
    {
        return found->second;
    }
    else
    {
        // Instead of creating a inconsistant configuration, warn and return a default one
        // default configuration is defined as a static member of ShaderConfiguration
        LOG(logWARNING) << "ShaderConfiguration \"" << name << "\" has not been registered. Return default (added to the factory).";
/*
        const ShaderConfiguration config(name);
        configs.insert(std::make_pair(name, config));
*/
        ShaderConfiguration config = ShaderConfiguration::getDefaultShaderConfig();
        configs.insert(std::make_pair(config.m_name, config));
        return config;
    }
}

}
}
}
