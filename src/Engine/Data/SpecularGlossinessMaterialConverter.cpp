#include <Core/Material/SpecularGlossinessMaterialData.hpp>
#include <Engine/Data/SpecularGlossinessMaterial.hpp>
#include <Engine/Data/SpecularGlossinessMaterialConverter.hpp>

namespace Ra {
namespace Engine {
namespace Data {

using namespace Ra::Core::Asset;

Material* SpecularGlossinessMaterialConverter::operator()( const MaterialData* toconvert ) {
    auto result = new SpecularGlossiness( toconvert->getName() );
    auto source = static_cast<const Core::Material::SpecularGlossinessData*>( toconvert );

    result->fillBaseFrom( source );

    result->m_diffuseFactor = source->m_diffuseFactor;
    if ( source->m_hasDiffuseTexture ) {
        result->addTexture( { "TEX_DIFFUSE" }, source->m_diffuseTexture, source->m_diffuseSampler );
        result->m_diffuseTextureTransform = std::move( source->m_diffuseTextureTransform );
    }
    result->m_specularFactor   = source->m_specularFactor;
    result->m_glossinessFactor = source->m_glossinessFactor;
    if ( source->m_hasSpecularGlossinessTexture ) {
        result->addTexture( { "TEX_SPECULARGLOSSINESS" },
                            source->m_specularGlossinessTexture,
                            source->m_specularGlossinessSampler );
        result->m_specularGlossinessTransform = std::move( source->m_specularGlossinessTransform );
    }

    return result;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
