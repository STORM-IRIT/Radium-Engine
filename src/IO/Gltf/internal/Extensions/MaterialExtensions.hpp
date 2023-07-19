#pragma once
#include <IO/Gltf/internal/fx/gltf.h>

/**
 * Add default values to the namespace containing all default values.
 * @note : it is not a good idea to add to external namespace
 */
namespace fx::gltf::defaults {
constexpr std::array<float, 2> IdentityVec2 { 1, 1 };
constexpr std::array<float, 2> NullVec2 { 0, 0 };
} // namespace fx::gltf::defaults

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_pbrSpecularGlossiness
 *
 */
struct gltf_PBRSpecularGlossiness {
    std::array<float, 4> diffuseFactor { fx::gltf::defaults::IdentityVec4 };
    fx::gltf::Material::Texture diffuseTexture;

    float glossinessFactor { fx::gltf::defaults::IdentityScalar };
    std::array<float, 3> specularFactor { fx::gltf::defaults::IdentityVec3 };
    fx::gltf::Material::Texture specularGlossinessTexture;

    nlohmann::json extensionsAndExtras {};

    [[nodiscard]] bool empty() const {
        return diffuseTexture.empty() && diffuseFactor == fx::gltf::defaults::IdentityVec4 &&
               glossinessFactor == fx::gltf::defaults::IdentityScalar &&
               specularFactor == fx::gltf::defaults::IdentityVec3 &&
               specularGlossinessTexture.empty();
    }
};

inline void from_json( nlohmann::json const& json,
                       gltf_PBRSpecularGlossiness& pbrSpecularGlossiness ) {
    fx::gltf::detail::ReadOptionalField(
        "diffuseFactor", json, pbrSpecularGlossiness.diffuseFactor );
    fx::gltf::detail::ReadOptionalField(
        "diffuseTexture", json, pbrSpecularGlossiness.diffuseTexture );
    fx::gltf::detail::ReadOptionalField(
        "glossinessFactor", json, pbrSpecularGlossiness.glossinessFactor );
    fx::gltf::detail::ReadOptionalField(
        "specularFactor", json, pbrSpecularGlossiness.specularFactor );
    fx::gltf::detail::ReadOptionalField(
        "specularGlossinessTexture", json, pbrSpecularGlossiness.specularGlossinessTexture );

    fx::gltf::detail::ReadExtensionsAndExtras( json, pbrSpecularGlossiness.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json,
                     gltf_PBRSpecularGlossiness const& pbrSpecularGlossiness ) {
    fx::gltf::detail::WriteField( { "diffuseFactor" },
                                  json,
                                  pbrSpecularGlossiness.diffuseFactor,
                                  fx::gltf::defaults::IdentityVec4 );
    fx::gltf::detail::WriteField(
        { "diffuseTexture" }, json, pbrSpecularGlossiness.diffuseTexture );
    fx::gltf::detail::WriteField( { "glossinessFactor" },
                                  json,
                                  pbrSpecularGlossiness.glossinessFactor,
                                  fx::gltf::defaults::IdentityScalar );
    fx::gltf::detail::WriteField( { "specularFactor" },
                                  json,
                                  pbrSpecularGlossiness.specularFactor,
                                  fx::gltf::defaults::IdentityVec3 );
    fx::gltf::detail::WriteField(
        { "specularGlossinessTexture" }, json, pbrSpecularGlossiness.specularGlossinessTexture );
    fx::gltf::detail::WriteExtensions( json, pbrSpecularGlossiness.extensionsAndExtras );
}

/**
 * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_texture_transform
 * KHR_texture_transform
 */
struct gltf_KHRTextureTransform {
    std::array<float, 2> offset { fx::gltf::defaults::NullVec2 };
    std::array<float, 2> scale { fx::gltf::defaults::IdentityVec2 };
    float rotation { 0.0 };
    int texCoord { -1 };
    nlohmann::json extensionsAndExtras {};

    bool isDefault() {
        return ( rotation == 0.0 ) && ( texCoord == -1 ) &&
               ( offset == fx::gltf::defaults::NullVec2 ) &&
               ( scale == fx::gltf::defaults::IdentityVec2 ) && extensionsAndExtras.is_null();
    }
};

inline void from_json( nlohmann::json const& json, gltf_KHRTextureTransform& khrTextureTransform ) {
    fx::gltf::detail::ReadOptionalField( "offset", json, khrTextureTransform.offset );
    fx::gltf::detail::ReadOptionalField( "scale", json, khrTextureTransform.scale );
    fx::gltf::detail::ReadOptionalField( "rotation", json, khrTextureTransform.rotation );
    fx::gltf::detail::ReadOptionalField( "texCoord", json, khrTextureTransform.texCoord );

    fx::gltf::detail::ReadExtensionsAndExtras( json, khrTextureTransform.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json, gltf_KHRTextureTransform const& khrTextureTransform ) {
    fx::gltf::detail::WriteField(
        { "offset" }, json, khrTextureTransform.offset, fx::gltf::defaults::NullVec2 );
    fx::gltf::detail::WriteField(
        { "scale" }, json, khrTextureTransform.scale, fx::gltf::defaults::IdentityVec2 );
    fx::gltf::detail::WriteField( { "rotation" }, json, khrTextureTransform.rotation, 0.f );
    fx::gltf::detail::WriteField( { "texCoord" }, json, khrTextureTransform.texCoord, -1 );

    fx::gltf::detail::WriteExtensions( json, khrTextureTransform.extensionsAndExtras );
}

/**
 * KHR_materials_ior
 * https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_ior
 */
struct gltf_KHRMaterialsIor {
    float ior { 1.5 };
    nlohmann::json extensionsAndExtras {};
};

inline void from_json( nlohmann::json const& json, gltf_KHRMaterialsIor& khrMaterialsIor ) {
    fx::gltf::detail::ReadOptionalField( "ior", json, khrMaterialsIor.ior );
    fx::gltf::detail::ReadExtensionsAndExtras( json, khrMaterialsIor.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json, gltf_KHRMaterialsIor const& khrMaterialsIor ) {
    fx::gltf::detail::WriteField( { "ior" }, json, khrMaterialsIor.ior, 1.5f );

    fx::gltf::detail::WriteExtensions( json, khrMaterialsIor.extensionsAndExtras );
}

/**
 * KHR_materials_clearcoat
 * https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_clearcoat
 */
struct gltf_KHRMaterialsClearcoat {
    float clearcoatFactor { 0. };
    fx::gltf::Material::Texture clearcoatTexture;

    float clearcoatRoughnessFactor { 0. };
    fx::gltf::Material::Texture clearcoatRoughnessTexture;

    fx::gltf::Material::NormalTexture clearcoatNormalTexture;

    nlohmann::json extensionsAndExtras {};

    bool isDefault() {
        return ( clearcoatFactor == 0.0 ) && ( clearcoatRoughnessFactor == 0.0 ) &&
               clearcoatTexture.empty() && clearcoatRoughnessTexture.empty() &&
               clearcoatNormalTexture.empty() && extensionsAndExtras.is_null();
    }
};

inline void from_json( nlohmann::json const& json,
                       gltf_KHRMaterialsClearcoat& khrMaterialsClearcoat ) {
    fx::gltf::detail::ReadOptionalField(
        "clearcoatFactor", json, khrMaterialsClearcoat.clearcoatFactor );
    fx::gltf::detail::ReadOptionalField(
        "clearcoatTexture", json, khrMaterialsClearcoat.clearcoatTexture );

    fx::gltf::detail::ReadOptionalField(
        "clearcoatRoughnessFactor", json, khrMaterialsClearcoat.clearcoatRoughnessFactor );
    fx::gltf::detail::ReadOptionalField(
        "clearcoatRoughnessTexture", json, khrMaterialsClearcoat.clearcoatRoughnessTexture );

    fx::gltf::detail::ReadOptionalField(
        "clearcoatNormalTexture", json, khrMaterialsClearcoat.clearcoatNormalTexture );

    fx::gltf::detail::ReadExtensionsAndExtras( json, khrMaterialsClearcoat.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json,
                     gltf_KHRMaterialsClearcoat const& khrMaterialsClearcoat ) {
    fx::gltf::detail::WriteField(
        { "clearcoatFactor" }, json, khrMaterialsClearcoat.clearcoatFactor, 0.f );
    if ( !khrMaterialsClearcoat.clearcoatTexture.empty() ) {
        fx::gltf::detail::WriteField(
            { "clearcoatTexture" }, json, khrMaterialsClearcoat.clearcoatTexture );
    }

    fx::gltf::detail::WriteField(
        { "clearcoatRoughnessFactor" }, json, khrMaterialsClearcoat.clearcoatRoughnessFactor, 0.f );
    if ( !khrMaterialsClearcoat.clearcoatRoughnessTexture.empty() ) {
        fx::gltf::detail::WriteField( { "clearcoatRoughnessTexture" },
                                      json,
                                      khrMaterialsClearcoat.clearcoatRoughnessTexture );
    }

    if ( !khrMaterialsClearcoat.clearcoatNormalTexture.empty() ) {
        fx::gltf::detail::WriteField(
            { "clearcoatNormalTexture" }, json, khrMaterialsClearcoat.clearcoatNormalTexture );
    }

    fx::gltf::detail::WriteExtensions( json, khrMaterialsClearcoat.extensionsAndExtras );
}

/**
 * KHR_materials_specular
 * https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_specular
 */
struct gltf_KHRMaterialsSpecular {
    float specularFactor { 1. };
    fx::gltf::Material::Texture specularTexture;

    std::array<float, 3> specularColorFactor { fx::gltf::defaults::IdentityVec3 };
    fx::gltf::Material::Texture specularColorTexture;

    nlohmann::json extensionsAndExtras {};

    bool isDefault() {
        return ( specularFactor == 1.0 ) &&
               ( specularColorFactor == fx::gltf::defaults::IdentityVec3 ) &&
               specularTexture.empty() && specularColorTexture.empty() &&
               extensionsAndExtras.is_null();
    }
};

inline void from_json( nlohmann::json const& json,
                       gltf_KHRMaterialsSpecular& khrMaterialsSpecular ) {
    fx::gltf::detail::ReadOptionalField(
        "specularFactor", json, khrMaterialsSpecular.specularFactor );
    fx::gltf::detail::ReadOptionalField(
        "specularTexture", json, khrMaterialsSpecular.specularTexture );

    fx::gltf::detail::ReadOptionalField(
        "specularColorFactor", json, khrMaterialsSpecular.specularColorFactor );
    fx::gltf::detail::ReadOptionalField(
        "specularColorTexture", json, khrMaterialsSpecular.specularColorTexture );

    fx::gltf::detail::ReadExtensionsAndExtras( json, khrMaterialsSpecular.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json, gltf_KHRMaterialsSpecular const& khrMaterialsSpecular ) {
    fx::gltf::detail::WriteField(
        { "specularFactor" }, json, khrMaterialsSpecular.specularFactor, 1.f );
    if ( !khrMaterialsSpecular.specularTexture.empty() ) {
        fx::gltf::detail::WriteField(
            { "specularTexture" }, json, khrMaterialsSpecular.specularTexture );
    }

    fx::gltf::detail::WriteField( { "specularColorFactor" },
                                  json,
                                  khrMaterialsSpecular.specularColorFactor,
                                  fx::gltf::defaults::IdentityVec3 );
    if ( !khrMaterialsSpecular.specularColorTexture.empty() ) {
        fx::gltf::detail::WriteField(
            { "specularColorTexture" }, json, khrMaterialsSpecular.specularColorTexture );
    }

    fx::gltf::detail::WriteExtensions( json, khrMaterialsSpecular.extensionsAndExtras );
}

/**
 * KHR_materials_sheen
 * https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_sheen
 */
struct gltf_KHRMaterialsSheen {
    std::array<float, 3> sheenColorFactor { fx::gltf::defaults::NullVec3 };
    fx::gltf::Material::Texture sheenColorTexture;

    float sheenRoughnessFactor { 0. };
    fx::gltf::Material::Texture sheenRoughnessTexture;

    nlohmann::json extensionsAndExtras {};

    bool isDefault() {
        return ( sheenRoughnessFactor == 0.0 ) &&
               ( sheenColorFactor == fx::gltf::defaults::NullVec3 ) && sheenColorTexture.empty() &&
               sheenRoughnessTexture.empty() && extensionsAndExtras.is_null();
    }
};

inline void from_json( nlohmann::json const& json, gltf_KHRMaterialsSheen& khrMaterialsSheen ) {
    fx::gltf::detail::ReadOptionalField(
        "sheenColorFactor", json, khrMaterialsSheen.sheenColorFactor );
    fx::gltf::detail::ReadOptionalField(
        "sheenColorTexture", json, khrMaterialsSheen.sheenColorTexture );

    fx::gltf::detail::ReadOptionalField(
        "sheenRoughnessFactor", json, khrMaterialsSheen.sheenRoughnessFactor );
    fx::gltf::detail::ReadOptionalField(
        "sheenRoughnessTexture", json, khrMaterialsSheen.sheenRoughnessTexture );

    fx::gltf::detail::ReadExtensionsAndExtras( json, khrMaterialsSheen.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json, gltf_KHRMaterialsSheen const& khrMaterialsSheen ) {
    fx::gltf::detail::WriteField( { "sheenColorFactor" },
                                  json,
                                  khrMaterialsSheen.sheenColorFactor,
                                  fx::gltf::defaults::NullVec3 );
    if ( !khrMaterialsSheen.sheenColorTexture.empty() ) {
        fx::gltf::detail::WriteField(
            { "sheenColorTexture" }, json, khrMaterialsSheen.sheenColorTexture );
    }

    fx::gltf::detail::WriteField(
        { "sheenRoughnessFactor" }, json, khrMaterialsSheen.sheenRoughnessFactor, 0.f );
    if ( !khrMaterialsSheen.sheenRoughnessTexture.empty() ) {
        fx::gltf::detail::WriteField(
            { "sheenRoughnessTexture" }, json, khrMaterialsSheen.sheenRoughnessTexture );
    }

    fx::gltf::detail::WriteExtensions( json, khrMaterialsSheen.extensionsAndExtras );
}

/**
 * INN_material_atlas_V1
 *
 */
struct gltf_INNMaterialAtlas {
    struct INN_AtlasTexture : fx::gltf::Material::Texture {
        int nbMaterial { 0 };
    };

    INN_AtlasTexture atlasTexture;
};

inline void from_json( nlohmann::json const& json,
                       gltf_INNMaterialAtlas::INN_AtlasTexture& atlasTexture ) {
    from_json( json, static_cast<fx::gltf::Material::Texture&>( atlasTexture ) );
    fx::gltf::detail::ReadRequiredField( "nbMaterial", json, atlasTexture.nbMaterial );
}

inline void from_json( nlohmann::json const& json, gltf_INNMaterialAtlas& textureAtlas ) {
    fx::gltf::detail::ReadRequiredField( "atlasTexture", json, textureAtlas.atlasTexture );
}

inline void to_json( nlohmann::json& json,
                     gltf_INNMaterialAtlas::INN_AtlasTexture const& atlasTexture ) {
    to_json( json, static_cast<fx::gltf::Material::Texture const&>( atlasTexture ) );
    fx::gltf::detail::WriteField( { "nbMaterial" }, json, atlasTexture.nbMaterial, 0 );
}

inline void to_json( nlohmann::json& json, gltf_INNMaterialAtlas const& textureAtlas ) {
    fx::gltf::detail::WriteField( { "atlasTexture" }, json, textureAtlas.atlasTexture );
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
