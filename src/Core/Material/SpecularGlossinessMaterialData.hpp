#pragma once

#include <Core/Material/BaseGLTFMaterial.hpp>

#include <map>

namespace Ra {
namespace Core {
namespace Material {

/**
 * \brief RadiumIO representation of Specular-Glossiness material
 * \note Specular-Glossiness is an archived extension of the specification. Its use is discouraged
 */
class RA_CORE_API SpecularGlossinessData : public BaseGLTFMaterial
{
  public:
    /// Diffuse texture
    std::string m_diffuseTexture {};
    Ra::Core::Utils::Color m_diffuseFactor { 1.0, 1.0, 1.0, 1.0 };
    GLTFSampler m_diffuseSampler {};
    bool m_hasDiffuseTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_diffuseTextureTransform { nullptr };

    /// Specular-Glossiness texture
    std::string m_specularGlossinessTexture {};
    Ra::Core::Utils::Color m_specularFactor { 1.0, 1.0, 1.0, 1.0 };
    float m_glossinessFactor { 1 };
    GLTFSampler m_specularGlossinessSampler {};
    bool m_hasSpecularGlossinessTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_specularGlossinessTransform { nullptr };

    explicit SpecularGlossinessData( const std::string& name = std::string {} );
    ~SpecularGlossinessData() override = default;
};

} // namespace Material
} // namespace Core
} // namespace Ra
