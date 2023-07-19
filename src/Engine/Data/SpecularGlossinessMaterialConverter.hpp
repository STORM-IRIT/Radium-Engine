#pragma once
#include <Engine/RaEngine.hpp>

#include <Core/Asset/MaterialData.hpp>
#include <Engine/Data/Material.hpp>

namespace Ra {
namespace Engine {
namespace Data {
/**
 * Radium IO to Engine conversion for pbrSpecularGlossiness
 */
class RA_ENGINE_API SpecularGlossinessMaterialConverter
{
  public:
    SpecularGlossinessMaterialConverter() = default;

    ~SpecularGlossinessMaterialConverter() = default;

    Ra::Engine::Data::Material* operator()( const Ra::Core::Asset::MaterialData* toconvert );
};

} // namespace Data
} // namespace Engine
} // namespace Ra
