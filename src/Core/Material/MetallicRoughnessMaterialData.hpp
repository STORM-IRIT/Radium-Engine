#pragma once

#include <Core/Material/BaseGLTFMaterial.hpp>

#include <map>

namespace Ra {
namespace Core {
namespace Material {

/**
 * \brief RadiumIO representation of the MetalicRoughness material
 */
class RA_CORE_API MetallicRoughnessData : public BaseGLTFMaterial
{
  public:
    /// Base texture
    std::string m_baseColorTexture {};
    Ra::Core::Utils::Color m_baseColorFactor { 1.0, 1.0, 1.0, 1.0 };
    GLTFSampler m_baseSampler {};
    bool m_hasBaseColorTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_baseTextureTransform { nullptr };

    /// Metallic-Roughness texture
    std::string m_metallicRoughnessTexture {};
    float m_metallicFactor { 1 };
    float m_roughnessFactor { 1 };
    GLTFSampler m_metallicRoughnessSampler {};
    bool m_hasMetallicRoughnessTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_metallicRoughnessTextureTransform { nullptr };

    explicit MetallicRoughnessData( const std::string& name = std::string {} );
    ~MetallicRoughnessData() override = default;
};

} // namespace Material
} // namespace Core
} // namespace Ra
