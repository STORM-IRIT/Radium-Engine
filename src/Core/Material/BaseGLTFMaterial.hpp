#pragma once
#include <Core/RaCore.hpp>

#include <Core/Asset/MaterialData.hpp>
#include <Core/Material/GLTFTextureParameters.hpp>
#include <Core/Utils/Color.hpp>

#include <map>
namespace Ra {
namespace Core {
namespace Material {

/// GLTF Alpha mode definition
enum AlphaMode : unsigned int { Opaque = 0, Mask, Blend };

/**
 * \brief Base class for all official gltf extensions.
 *
 * Official gltf extensions are listed and specified at
 * https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0
 */
struct RA_CORE_API GLTFMaterialExtensionData : public Ra::Core::Asset::MaterialData {
    explicit GLTFMaterialExtensionData( const std::string& gltfType,
                                        const std::string& instanceName ) :
        Ra::Core::Asset::MaterialData( instanceName, gltfType ) {}
    GLTFMaterialExtensionData() = delete;
};

/**
 * \brief Definition of the base gltf material representation
 */
class RA_CORE_API BaseGLTFMaterial : public Ra::Core::Asset::MaterialData
{
  public:
    /// Attributes of a gltf material
    /// Normal texture
    std::string m_normalTexture {};
    float m_normalTextureScale { 1 };
    GLTFSampler m_normalSampler {};
    bool m_hasNormalTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_normalTextureTransform { nullptr };

    /// Occlusion texture
    std::string m_occlusionTexture {};
    float m_occlusionStrength { 1 };
    GLTFSampler m_occlusionSampler {};
    bool m_hasOcclusionTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_occlusionTextureTransform { nullptr };
    /// Emissive texture
    std::string m_emissiveTexture {};
    Ra::Core::Utils::Color m_emissiveFactor { 0.0, 0.0, 0.0, 1.0 };
    GLTFSampler m_emissiveSampler {};
    bool m_hasEmissiveTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_emissiveTextureTransform { nullptr };

    /// Transparency parameters
    AlphaMode m_alphaMode { AlphaMode::Opaque };
    float m_alphaCutoff { 0.5 };

    /// Face culling parameter
    bool m_doubleSided { false };

    // Extension data pass through the system
    std::map<std::string, std::unique_ptr<GLTFMaterialExtensionData>> m_extensions {};

    explicit BaseGLTFMaterial( const std::string& gltfType, const std::string& instanceName );
    ~BaseGLTFMaterial() override = default;

    virtual bool supportExtension( const std::string& extensionName ) {
        auto it = m_allowedExtensions.find( extensionName );
        return ( it != m_allowedExtensions.end() ) && ( it->second );
    }

    void prohibitAllExtensions() { m_allowedExtensions.clear(); }

    void prohibitExtension( const std::string& extension ) {
        m_allowedExtensions[extension] = false;
    }

    void allowExtension( const std::string& extension ) { m_allowedExtensions[extension] = true; }

  private:
    std::map<std::string, bool> m_allowedExtensions {};
};

/**
 * \brief Clearcoat layer extension
 */
struct RA_CORE_API GLTFClearcoatLayer : public GLTFMaterialExtensionData {
    explicit GLTFClearcoatLayer( const std::string& name = std::string {} ) :
        GLTFMaterialExtensionData( "Clearcoat", name ) {}
    /// The clearcoat layer intensity.
    float m_clearcoatFactor { 0 };
    bool m_hasClearcoatTexture { false };
    std::string m_clearcoatTexture {};
    GLTFSampler m_clearcoatSampler {};
    mutable std::unique_ptr<GLTFTextureTransform> m_clearcoatTextureTransform { nullptr };
    /// The clearcoat layer roughness.
    float m_clearcoatRoughnessFactor { 0 };
    bool m_hasClearcoatRoughnessTexture { false };
    std::string m_clearcoatRoughnessTexture {};
    GLTFSampler m_clearcoatRoughnessSampler {};
    mutable std::unique_ptr<GLTFTextureTransform> m_clearcoatRoughnessTextureTransform { nullptr };
    /// The clearcoat normal map texture.
    std::string m_clearcoatNormalTexture {};
    float m_clearcoatNormalTextureScale { 1 };
    GLTFSampler m_clearcoatNormalSampler {};
    bool m_hasClearcoatNormalTexture { false };
    mutable std::unique_ptr<GLTFTextureTransform> m_clearcoatNormalTextureTransform { nullptr };
};

/**
 * \brief Specular layer extension
 */
struct RA_CORE_API GLTFSpecularLayer : public GLTFMaterialExtensionData {
    explicit GLTFSpecularLayer( const std::string& name = std::string {} ) :
        GLTFMaterialExtensionData( "Specular", name ) {}
    /// The specular layer strength.
    float m_specularFactor { 1. };
    bool m_hasSpecularTexture { false };
    std::string m_specularTexture {};
    GLTFSampler m_specularSampler {};
    mutable std::unique_ptr<GLTFTextureTransform> m_specularTextureTransform { nullptr };
    /// The specular layer color.
    Ra::Core::Utils::Color m_specularColorFactor { 1.0, 1.0, 1.0, 1.0 };
    bool m_hasSpecularColorTexture { false };
    std::string m_specularColorTexture {};
    GLTFSampler m_specularColorSampler {};
    mutable std::unique_ptr<GLTFTextureTransform> m_specularColorTextureTransform { nullptr };
};

/**
 * \brief Sheen layer extension
 */
struct RA_CORE_API GLTFSheenLayer : public GLTFMaterialExtensionData {
    explicit GLTFSheenLayer( const std::string& name = std::string {} ) :
        GLTFMaterialExtensionData( "Sheen", name ) {}

    /// The sheen color.
    Ra::Core::Utils::Color m_sheenColorFactor { 0.0, 0.0, 0.0, 1.0 };
    bool m_hasSheenColorTexture { false };
    std::string m_sheenColorTexture {};
    GLTFSampler m_sheenColorTextureSampler {};
    mutable std::unique_ptr<GLTFTextureTransform> m_sheenColorTextureTransform { nullptr };

    /// The sheen roughness.
    float m_sheenRoughnessFactor { 0 };
    bool m_hasSheenRoughnessTexture { false };
    std::string m_sheenRoughnessTexture {};
    GLTFSampler m_sheenRoughnessTextureSampler {};
    mutable std::unique_ptr<GLTFTextureTransform> m_sheenRoughnessTextureTransform { nullptr };
};

/**
 * \brief IOR extension
 */
struct RA_CORE_API GLTFIor : public GLTFMaterialExtensionData {
    explicit GLTFIor( const std::string& name = std::string {} ) :
        GLTFMaterialExtensionData( "Ior", name ) {}
    float m_ior { 1.5 };
};

} // namespace Material
} // namespace Core
} // namespace Ra
