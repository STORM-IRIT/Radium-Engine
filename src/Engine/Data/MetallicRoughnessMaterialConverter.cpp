#include <Core/Material/MetallicRoughnessMaterialData.hpp>
#include <Engine/Data/MetallicRoughnessMaterial.hpp>
#include <Engine/Data/MetallicRoughnessMaterialConverter.hpp>

namespace Ra {
namespace Engine {
namespace Data {
using namespace Ra::Core::Asset;

Material*
MetallicRoughnessMaterialConverter::operator()( const Ra::Core::Asset::MaterialData* toconvert ) {
    auto result = new MetallicRoughness( toconvert->getName() );
    auto source = static_cast<const Core::Material::MetallicRoughnessData*>( toconvert );

    result->fillBaseFrom( source );

    result->m_baseColorFactor = source->m_baseColorFactor;
    if ( source->m_hasBaseColorTexture ) {
        result->addTexture(
            { "TEX_BASECOLOR" }, source->m_baseColorTexture, source->m_baseSampler );
        result->m_baseTextureTransform = std::move( source->m_baseTextureTransform );
    }
    result->m_metallicFactor  = source->m_metallicFactor;
    result->m_roughnessFactor = source->m_roughnessFactor;
    if ( source->m_hasMetallicRoughnessTexture ) {
        result->addTexture( { "TEX_METALLICROUGHNESS" },
                            source->m_metallicRoughnessTexture,
                            source->m_metallicRoughnessSampler );
        result->m_metallicRoughnessTextureTransform =
            std::move( source->m_metallicRoughnessTextureTransform );
    }

    return result;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
