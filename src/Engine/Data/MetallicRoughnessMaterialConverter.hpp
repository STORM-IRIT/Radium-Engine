#pragma once

#include <Engine/RaEngine.hpp>

#include <Core/Asset/MaterialData.hpp>
#include <Engine/Data/Material.hpp>

namespace Ra {
namespace Engine {
namespace Data {
/**
 * Radium IO to Engine conversion for pbrMetallicRoughness
 */
class RA_ENGINE_API MetallicRoughnessMaterialConverter
{
  public:
    MetallicRoughnessMaterialConverter() = default;

    ~MetallicRoughnessMaterialConverter() = default;

    Ra::Engine::Data::Material* operator()( const Ra::Core::Asset::MaterialData* toconvert );
};

} // namespace Data
} // namespace Engine
} // namespace Ra
