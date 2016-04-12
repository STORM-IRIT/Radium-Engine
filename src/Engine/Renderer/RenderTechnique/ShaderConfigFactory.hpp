#ifndef RADIUMENGINE_SHADERCONFIGFACTORY_HPP
#define RADIUMENGINE_SHADERCONFIGFACTORY_HPP

#include <string>

#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

namespace Ra
{
namespace Engine
{

namespace ShaderConfigurationFactory
{
    RA_ENGINE_API void addConfiguration(const ShaderConfiguration& config);
    RA_ENGINE_API void addConfiguration(const std::string& name, const ShaderConfiguration& config);
    RA_ENGINE_API ShaderConfiguration getConfiguration(const std::string& name);
}

} // namespace Engine
} // namespace Ra


#endif // RADIUMENGINE_SHADERCONFIGFACTORY_HPP
