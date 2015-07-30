#ifndef FANCYMESHPLUGIN_FANCYMESHLOADER_HPP
#define FANCYMESHPLUGIN_FANCYMESHLOADER_HPP

#include <string>
#include <vector>

#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoadingData.hpp>

namespace Ra {
    typedef std::vector<Engine::FancyComponentData, Core::AlignedAllocator<Engine::FancyComponentData, 16> >DataVector;
}
namespace Ra { namespace Engine {

namespace FancyMeshLoader
{
RA_API DataVector loadFile(const std::string& name);

} // namespace FancyMeshLoader;

} // namespace Engine
} // namespace Ra

#endif // FANCYMESHPLUGIN_FANCYMESHLOADER_HPP
