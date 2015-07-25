#ifndef FANCYMESHPLUGIN_FANCYMESHLOADER_HPP
#define FANCYMESHPLUGIN_FANCYMESHLOADER_HPP

#include <string>
#include <vector>

#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoadingData.hpp>

namespace Ra { namespace Engine {

namespace FancyMeshLoader
{

std::vector<FancyComponentData> loadFile(const std::string& name);

} // namespace FancyMeshLoader;

} // namespace Engine
} // namespace Ra

#endif // FANCYMESHPLUGIN_FANCYMESHLOADER_HPP
