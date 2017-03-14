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
        LOG(logWARNING) << "ShaderConfiguration \"" << name << "\" has not been registered. Create it.";
        const ShaderConfiguration config(name);
        configs.insert(std::make_pair(name, config));
        return config;
    }
}

}
}
}
