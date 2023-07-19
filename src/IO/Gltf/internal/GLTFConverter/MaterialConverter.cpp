#include <Core/Material/MetallicRoughnessMaterialData.hpp>
#include <Core/Material/SpecularGlossinessMaterialData.hpp>
#include <Core/Utils/Log.hpp>

#include <IO/Gltf/internal/GLTFConverter/ImageData.hpp>
#include <IO/Gltf/internal/GLTFConverter/MaterialConverter.hpp>

#include <IO/Gltf/internal/Extensions/MaterialExtensions.hpp>

using namespace fx;

namespace Ra {
namespace IO {
namespace GLTF {
using namespace Ra::Core::Asset;
using namespace Ra::Core::Utils;
using namespace Ra::Core::Material;

GLTFSampler convertSampler( const fx::gltf::Sampler& sampler ) {
    GLTFSampler rasampler;
    switch ( sampler.magFilter ) {
    case fx::gltf::Sampler::MagFilter::Nearest:
        rasampler.magFilter = GLTFSampler::MagFilter::Nearest;
        break;
    case fx::gltf::Sampler::MagFilter::Linear:
        rasampler.magFilter = GLTFSampler::MagFilter::Linear;
        break;
    default:
        rasampler.magFilter = GLTFSampler::MagFilter::Nearest;
        break;
    }
    switch ( sampler.minFilter ) {
    case fx::gltf::Sampler::MinFilter::Nearest:
        rasampler.minFilter = GLTFSampler::MinFilter::Nearest;
        break;
    case fx::gltf::Sampler::MinFilter::Linear:
        rasampler.minFilter = GLTFSampler::MinFilter::Linear;
        break;
    case fx::gltf::Sampler::MinFilter::NearestMipMapNearest:
        rasampler.minFilter = GLTFSampler::MinFilter::NearestMipMapNearest;
        break;
    case fx::gltf::Sampler::MinFilter::LinearMipMapNearest:
        rasampler.minFilter = GLTFSampler::MinFilter::LinearMipMapNearest;
        break;
    case fx::gltf::Sampler::MinFilter::NearestMipMapLinear:
        rasampler.minFilter = GLTFSampler::MinFilter::NearestMipMapLinear;
        break;
    case fx::gltf::Sampler::MinFilter::LinearMipMapLinear:
        rasampler.minFilter = GLTFSampler::MinFilter::LinearMipMapLinear;
        break;
    default:
        rasampler.minFilter = GLTFSampler::MinFilter::Nearest;
        break;
    }
    switch ( sampler.wrapS ) {
    case fx::gltf::Sampler::WrappingMode::ClampToEdge:
        rasampler.wrapS = GLTFSampler::WrappingMode ::ClampToEdge;
        break;
    case fx::gltf::Sampler::WrappingMode::MirroredRepeat:
        rasampler.wrapS = GLTFSampler::WrappingMode ::MirroredRepeat;
        break;
    case fx::gltf::Sampler::WrappingMode::Repeat:
        rasampler.wrapS = GLTFSampler::WrappingMode ::Repeat;
        break;
    }
    switch ( sampler.wrapT ) {
    case fx::gltf::Sampler::WrappingMode::ClampToEdge:
        rasampler.wrapT = GLTFSampler::WrappingMode ::ClampToEdge;
        break;
    case fx::gltf::Sampler::WrappingMode::MirroredRepeat:
        rasampler.wrapT = GLTFSampler::WrappingMode ::MirroredRepeat;
        break;
    case fx::gltf::Sampler::WrappingMode::Repeat:
        rasampler.wrapT = GLTFSampler::WrappingMode ::Repeat;
        break;
    }

    return rasampler;
}

void getMaterialExtensions( const nlohmann::json& extensionsAndExtras, BaseGLTFMaterial* mat ) {
    // Manage non standard material extensions
#if 0
    if ( !extensionsAndExtras.empty() ) {
        auto ext = extensionsAndExtras.find( "extensions" );
        if ( ext != extensionsAndExtras.end() ) {
            auto extensions                           = *ext;
            const nlohmann::json::const_iterator iter = extensions.find( "INN_material_atlas_V1" );
            if ( iter != extensions.end() ) {
                mat->m_inn_materialAtlas = new gltf_INNMaterialAtlas;
                from_json( *iter, *( mat->m_inn_materialAtlas ) );
            }
        }
    }
#endif
}

void getMaterialTextureTransform( const nlohmann::json& extensionsAndExtras,
                                  std::unique_ptr<GLTFTextureTransform>& dest ) {
    if ( !extensionsAndExtras.empty() ) {
        auto ext = extensionsAndExtras.find( "extensions" );
        if ( ext != extensionsAndExtras.end() ) {
            nlohmann::json textureExtensions    = *ext;
            nlohmann::json::const_iterator iter = textureExtensions.find( "KHR_texture_transform" );
            if ( iter != textureExtensions.end() ) {
                gltf_KHRTextureTransform textTransform;
                from_json( *iter, textTransform );
                dest            = std::make_unique<GLTFTextureTransform>();
                dest->offset[0] = textTransform.offset[0];
                dest->offset[1] = textTransform.offset[1];
                dest->scale[0]  = textTransform.scale[0];
                dest->scale[1]  = textTransform.scale[1];
                dest->rotation  = textTransform.rotation;
                dest->texCoord  = textTransform.texCoord;
            }
        }
    }
}

void getCommonMaterialParameters( const gltf::Document& doc,
                                  const std::string& filePath,
                                  const fx::gltf::Material& gltfMaterial,
                                  BaseGLTFMaterial* mat ) {
    // Normal texture
    if ( !gltfMaterial.normalTexture.empty() ) {
        ImageData tex { doc, gltfMaterial.normalTexture.index, filePath };
        if ( !tex.Info().IsBinary() ) {
            mat->m_normalTexture      = tex.Info().FileName;
            mat->m_normalTextureScale = gltfMaterial.normalTexture.scale;
            mat->m_hasNormalTexture   = true;
        }
        else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
        // get sampler information for this texture
        int samplerIndex = doc.textures[gltfMaterial.normalTexture.index].sampler;
        if ( samplerIndex >= 0 ) {
            mat->m_normalSampler = convertSampler( doc.samplers[samplerIndex] );
        }
        getMaterialTextureTransform( gltfMaterial.normalTexture.extensionsAndExtras,
                                     mat->m_normalTextureTransform );
    }
    // Occlusion texture
    if ( !gltfMaterial.occlusionTexture.empty() ) {
        ImageData tex { doc, gltfMaterial.occlusionTexture.index, filePath };
        if ( !tex.Info().IsBinary() ) {
            mat->m_occlusionTexture    = tex.Info().FileName;
            mat->m_occlusionStrength   = gltfMaterial.occlusionTexture.strength;
            mat->m_hasOcclusionTexture = true;
        }
        else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
        // get sampler information for this texture
        int samplerIndex = doc.textures[gltfMaterial.occlusionTexture.index].sampler;
        if ( samplerIndex >= 0 ) {
            mat->m_occlusionSampler = convertSampler( doc.samplers[samplerIndex] );
        }
        getMaterialTextureTransform( gltfMaterial.occlusionTexture.extensionsAndExtras,
                                     mat->m_occlusionTextureTransform );
    }
    // Emmissive Component
    mat->m_emissiveFactor = { gltfMaterial.emissiveFactor[0],
                              gltfMaterial.emissiveFactor[1],
                              gltfMaterial.emissiveFactor[2],
                              1_ra };
    if ( !gltfMaterial.emissiveTexture.empty() ) {
        ImageData tex { doc, gltfMaterial.emissiveTexture.index, filePath };
        if ( !tex.Info().IsBinary() ) {
            mat->m_emissiveTexture    = tex.Info().FileName;
            mat->m_hasEmissiveTexture = true;
        }
        else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
        // get sampler information for this texture
        int samplerIndex = doc.textures[gltfMaterial.emissiveTexture.index].sampler;
        if ( samplerIndex >= 0 ) {
            mat->m_emissiveSampler = convertSampler( doc.samplers[samplerIndex] );
        }
        getMaterialTextureTransform( gltfMaterial.emissiveTexture.extensionsAndExtras,
                                     mat->m_emissiveTextureTransform );
    }
    mat->m_alphaMode   = static_cast<GLTF::AlphaMode>( gltfMaterial.alphaMode );
    mat->m_doubleSided = gltfMaterial.doubleSided;
    mat->m_alphaCutoff = gltfMaterial.alphaCutoff;

    // load supported material extensions
    getMaterialExtensions( gltfMaterial.extensionsAndExtras, mat );
}

std::map<std::string,
         std::function<std::unique_ptr<GLTFMaterialExtensionData>( const gltf::Document& doc,
                                                                   const std::string& filePath,
                                                                   const nlohmann::json& jsonData,
                                                                   const std::string& basename )>>
    instanciateExtension {
        { "KHR_materials_ior",
          []( const gltf::Document& doc,
              const std::string& filePath,
              const nlohmann::json& jsonData,
              const std::string& basename ) {
              gltf_KHRMaterialsIor data;
              from_json( jsonData, data );
              auto built   = std::make_unique<GLTFIor>( basename + " - IOR" );
              built->m_ior = data.ior;
              return std::move( built );
          } },
        { "KHR_materials_clearcoat",
          []( const gltf::Document& doc,
              const std::string& filePath,
              const nlohmann::json& jsonData,
              const std::string& basename ) {
              gltf_KHRMaterialsClearcoat data;
              from_json( jsonData, data );
              auto built = std::make_unique<GLTFClearcoatLayer>( basename + " - Clearcoat layer" );
              // clearcoat layer
              built->m_clearcoatFactor = data.clearcoatFactor;
              if ( !data.clearcoatTexture.empty() ) {
                  ImageData tex { doc, data.clearcoatTexture.index, filePath };
                  if ( !tex.Info().IsBinary() ) {
                      built->m_clearcoatTexture    = tex.Info().FileName;
                      built->m_hasClearcoatTexture = true;
                  }
                  else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
                  // get sampler information for this texture
                  int samplerIndex = doc.textures[data.clearcoatTexture.index].sampler;
                  if ( samplerIndex >= 0 ) {
                      built->m_clearcoatSampler = convertSampler( doc.samplers[samplerIndex] );
                  }
                  getMaterialTextureTransform( data.clearcoatTexture.extensionsAndExtras,
                                               built->m_clearcoatTextureTransform );
              }
              // clearcoat roughness
              built->m_clearcoatRoughnessFactor = data.clearcoatRoughnessFactor;
              if ( !data.clearcoatRoughnessTexture.empty() ) {
                  ImageData tex { doc, data.clearcoatRoughnessTexture.index, filePath };
                  if ( !tex.Info().IsBinary() ) {
                      built->m_clearcoatRoughnessTexture    = tex.Info().FileName;
                      built->m_hasClearcoatRoughnessTexture = true;
                  }
                  else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
                  // get sampler information for this texture
                  int samplerIndex = doc.textures[data.clearcoatRoughnessTexture.index].sampler;
                  if ( samplerIndex >= 0 ) {
                      built->m_clearcoatRoughnessSampler =
                          convertSampler( doc.samplers[samplerIndex] );
                  }
                  getMaterialTextureTransform( data.clearcoatRoughnessTexture.extensionsAndExtras,
                                               built->m_clearcoatRoughnessTextureTransform );
              }
              // clearcoat Normal texture
              if ( !data.clearcoatNormalTexture.empty() ) {
                  ImageData tex { doc, data.clearcoatNormalTexture.index, filePath };
                  if ( !tex.Info().IsBinary() ) {
                      built->m_clearcoatNormalTexture      = tex.Info().FileName;
                      built->m_clearcoatNormalTextureScale = data.clearcoatNormalTexture.scale;
                      built->m_hasClearcoatNormalTexture   = true;
                  }
                  else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
                  // get sampler information for this texture
                  int samplerIndex = doc.textures[data.clearcoatNormalTexture.index].sampler;
                  if ( samplerIndex >= 0 ) {
                      built->m_clearcoatNormalSampler =
                          convertSampler( doc.samplers[samplerIndex] );
                  }
                  getMaterialTextureTransform( data.clearcoatNormalTexture.extensionsAndExtras,
                                               built->m_clearcoatNormalTextureTransform );
              }
              return std::move( built );
          } },
        { "KHR_materials_specular",
          []( const gltf::Document& doc,
              const std::string& filePath,
              const nlohmann::json& jsonData,
              const std::string& basename ) {
              gltf_KHRMaterialsSpecular data;
              from_json( jsonData, data );
              auto built = std::make_unique<GLTFSpecularLayer>( basename + " - Specular layer" );
              // spacular strength
              built->m_specularFactor = data.specularFactor;
              if ( !data.specularTexture.empty() ) {
                  ImageData tex { doc, data.specularTexture.index, filePath };
                  if ( !tex.Info().IsBinary() ) {
                      built->m_specularTexture    = tex.Info().FileName;
                      built->m_hasSpecularTexture = true;
                  }
                  else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
                  // get sampler information for this texture
                  int samplerIndex = doc.textures[data.specularTexture.index].sampler;
                  if ( samplerIndex >= 0 ) {
                      built->m_specularSampler = convertSampler( doc.samplers[samplerIndex] );
                  }
                  getMaterialTextureTransform( data.specularTexture.extensionsAndExtras,
                                               built->m_specularTextureTransform );
              }
              // specular color
              built->m_specularColorFactor = Ra::Core::Utils::Color { data.specularColorFactor[0],
                                                                      data.specularColorFactor[1],
                                                                      data.specularColorFactor[2],
                                                                      1_ra };
              if ( !data.specularColorTexture.empty() ) {
                  ImageData tex { doc, data.specularColorTexture.index, filePath };
                  if ( !tex.Info().IsBinary() ) {
                      built->m_specularColorTexture    = tex.Info().FileName;
                      built->m_hasSpecularColorTexture = true;
                  }
                  else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
                  // get sampler information for this texture
                  int samplerIndex = doc.textures[data.specularColorTexture.index].sampler;
                  if ( samplerIndex >= 0 ) {
                      built->m_specularColorSampler = convertSampler( doc.samplers[samplerIndex] );
                  }
                  getMaterialTextureTransform( data.specularColorTexture.extensionsAndExtras,
                                               built->m_specularColorTextureTransform );
              }
              return std::move( built );
          } },
        { "KHR_materials_sheen",
          []( const gltf::Document& doc,
              const std::string& filePath,
              const nlohmann::json& jsonData,
              const std::string& basename ) {
              gltf_KHRMaterialsSheen data;
              from_json( jsonData, data );
              auto built = std::make_unique<GLTFSheenLayer>( basename + " - Sheen layer" );
              // Sheen color.
              built->m_sheenColorFactor = Ra::Core::Utils::Color { data.sheenColorFactor[0],
                                                                   data.sheenColorFactor[1],
                                                                   data.sheenColorFactor[2],
                                                                   1_ra };
              if ( !data.sheenColorTexture.empty() ) {
                  ImageData tex { doc, data.sheenColorTexture.index, filePath };
                  if ( !tex.Info().IsBinary() ) {
                      built->m_sheenColorTexture    = tex.Info().FileName;
                      built->m_hasSheenColorTexture = true;
                  }
                  else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
                  // get sampler information for this texture
                  int samplerIndex = doc.textures[data.sheenColorTexture.index].sampler;
                  if ( samplerIndex >= 0 ) {
                      built->m_sheenColorTextureSampler =
                          convertSampler( doc.samplers[samplerIndex] );
                  }
                  getMaterialTextureTransform( data.sheenColorTexture.extensionsAndExtras,
                                               built->m_sheenColorTextureTransform );
              }
              // Sheen roughness
              built->m_sheenRoughnessFactor = data.sheenRoughnessFactor;
              if ( !data.sheenRoughnessTexture.empty() ) {
                  ImageData tex { doc, data.sheenRoughnessTexture.index, filePath };
                  if ( !tex.Info().IsBinary() ) {
                      built->m_sheenRoughnessTexture    = tex.Info().FileName;
                      built->m_hasSheenRoughnessTexture = true;
                  }
                  else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
                  // get sampler information for this texture
                  int samplerIndex = doc.textures[data.sheenRoughnessTexture.index].sampler;
                  if ( samplerIndex >= 0 ) {
                      built->m_sheenRoughnessTextureSampler =
                          convertSampler( doc.samplers[samplerIndex] );
                  }
                  getMaterialTextureTransform( data.sheenRoughnessTexture.extensionsAndExtras,
                                               built->m_sheenRoughnessTextureTransform );
              }
              return std::move( built );
          } } };

void getMaterialExtensions( const gltf::Document& doc,
                            const std::string& filePath,
                            const MaterialData& meshMaterial,
                            BaseGLTFMaterial* mat,
                            const std::vector<std::string> exept = {} ) {
    auto extensionsAndExtras = meshMaterial.Data().extensionsAndExtras;
    if ( !extensionsAndExtras.empty() ) {
        auto extensions = extensionsAndExtras.find( "extensions" );
        if ( extensions != extensionsAndExtras.end() ) {
            // first search for unlit extension becaus  it will prevent the use of other extensions
            // (Specification of 12/2021)
            // https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_unlit
            // Here, according to "Implementation Note: When KHR_materials_unlit is included with
            // another extension specifying a shading model on the same material, the result is
            // undefined", we choose to activate only the unlit extension
            if ( extensions->find( "KHR_materials_unlit" ) != extensions->end() ) {
                mat->prohibitAllExtensions();
                mat->allowExtension( "KHR_materials_unlit" );
            }
            // load supported extensions
            auto& extensionList = *extensions;
            for ( auto& [key, value] : extensionList.items() ) {
                if ( std::any_of( exept.begin(), exept.end(), [k = key]( const auto& e ) {
                         return e == k;
                     } ) ) {
                    continue;
                }
                if ( mat->supportExtension( key ) ) {
                    mat->m_extensions[key] =
                        instanciateExtension[key]( doc, filePath, value, mat->getName() );
                }
                else {
                    LOG( logINFO ) << "Extension " << key << " is NOT allowed for      "
                                   << mat->getType() << std::endl;
                }
            }
        }
    }
}

Ra::Core::Asset::MaterialData* buildDefaultMaterial( const gltf::Document& doc,
                                                     int32_t meshIndex,
                                                     const std::string& filePath,
                                                     int32_t meshPartNumber,
                                                     const MaterialData& meshMaterial ) {
    auto gltfMaterial = meshMaterial.Data();
    std::string materialName { gltfMaterial.name };
    if ( materialName.empty() ) { materialName = "material_"; }
    materialName += std::to_string( meshIndex ) + "_p_" + std::to_string( meshPartNumber );
    auto mat = new MetallicRoughnessData( materialName );
    getCommonMaterialParameters( doc, filePath, gltfMaterial, mat );
    getMaterialExtensions( doc, filePath, meshMaterial, mat );
    return mat;
}

Ra::Core::Asset::MaterialData* buildMetallicRoughnessMaterial( const gltf::Document& doc,
                                                               int32_t meshIndex,
                                                               const std::string& filePath,
                                                               int32_t meshPartNumber,
                                                               const MaterialData& meshMaterial ) {
    auto gltfMaterial = meshMaterial.Data();
    std::string materialName { gltfMaterial.name };
    if ( materialName.empty() ) {
        materialName = "material_";
        materialName += std::to_string( meshIndex ) + "_p_" + std::to_string( meshPartNumber );
    }

    auto mat = new MetallicRoughnessData( materialName );

    getCommonMaterialParameters( doc, filePath, gltfMaterial, mat );

    // Base color Component
    mat->m_baseColorFactor = { gltfMaterial.pbrMetallicRoughness.baseColorFactor[0],
                               gltfMaterial.pbrMetallicRoughness.baseColorFactor[1],
                               gltfMaterial.pbrMetallicRoughness.baseColorFactor[2],
                               gltfMaterial.pbrMetallicRoughness.baseColorFactor[3] };
    if ( !gltfMaterial.pbrMetallicRoughness.baseColorTexture.empty() ) {
        ImageData tex { doc, gltfMaterial.pbrMetallicRoughness.baseColorTexture.index, filePath };
        if ( !tex.Info().IsBinary() ) {
            mat->m_baseColorTexture    = tex.Info().FileName;
            mat->m_hasBaseColorTexture = true;
        }
        else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
        // get sampler information for this texture
        int samplerIndex =
            doc.textures[gltfMaterial.pbrMetallicRoughness.baseColorTexture.index].sampler;
        if ( samplerIndex >= 0 ) {
            mat->m_baseSampler = convertSampler( doc.samplers[samplerIndex] );
        }
        getMaterialTextureTransform(
            gltfMaterial.pbrMetallicRoughness.baseColorTexture.extensionsAndExtras,
            mat->m_baseTextureTransform );
    }

    // Metalic-Roughness  Component
    mat->m_metallicFactor  = gltfMaterial.pbrMetallicRoughness.metallicFactor;
    mat->m_roughnessFactor = gltfMaterial.pbrMetallicRoughness.roughnessFactor;
    if ( !gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.empty() ) {
        ImageData tex {
            doc, gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index, filePath };
        if ( !tex.Info().IsBinary() ) {
            mat->m_metallicRoughnessTexture    = tex.Info().FileName;
            mat->m_hasMetallicRoughnessTexture = true;
        }
        else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
        // get sampler information for this texture
        int samplerIndex =
            doc.textures[gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index].sampler;
        if ( samplerIndex >= 0 ) {
            mat->m_metallicRoughnessSampler = convertSampler( doc.samplers[samplerIndex] );
        }
        getMaterialTextureTransform(
            gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.extensionsAndExtras,
            mat->m_metallicRoughnessTextureTransform );
    }

    getMaterialExtensions( doc, filePath, meshMaterial, mat );
    return mat;
}

Ra::Core::Asset::MaterialData*
buildSpecularGlossinessMaterial( const gltf::Document& doc,
                                 int32_t meshIndex,
                                 const std::string& filePath,
                                 int32_t meshPartNumber,
                                 const MaterialData& meshMaterial,
                                 const gltf_PBRSpecularGlossiness& specularGloss ) {
    auto gltfMaterial = meshMaterial.Data();
    std::string materialName { gltfMaterial.name };
    if ( materialName.empty() ) { materialName = "material_"; }
    materialName += std::to_string( meshIndex ) + "_p_" + std::to_string( meshPartNumber );

    auto mat = new SpecularGlossinessData( materialName );

    getCommonMaterialParameters( doc, filePath, gltfMaterial, mat );

    // Diffuse color Component
    mat->m_diffuseFactor = { specularGloss.diffuseFactor[0],
                             specularGloss.diffuseFactor[1],
                             specularGloss.diffuseFactor[2],
                             specularGloss.diffuseFactor[3] };

    if ( !specularGloss.diffuseTexture.empty() ) {
        ImageData tex { doc, specularGloss.diffuseTexture.index, filePath };
        if ( !tex.Info().IsBinary() ) {
            mat->m_diffuseTexture    = tex.Info().FileName;
            mat->m_hasDiffuseTexture = true;
        }
        else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
        // get sampler information for this texture
        int samplerIndex = doc.textures[specularGloss.diffuseTexture.index].sampler;
        if ( samplerIndex >= 0 ) {
            mat->m_diffuseSampler = convertSampler( doc.samplers[samplerIndex] );
        }
        getMaterialTextureTransform( specularGloss.diffuseTexture.extensionsAndExtras,
                                     mat->m_diffuseTextureTransform );
    }

    // Specular-glossiness  Component
    mat->m_glossinessFactor = specularGloss.glossinessFactor;
    mat->m_specularFactor   = { specularGloss.specularFactor[0],
                                specularGloss.specularFactor[1],
                                specularGloss.specularFactor[2],
                                1_ra };
    if ( !specularGloss.specularGlossinessTexture.empty() ) {
        ImageData tex { doc, specularGloss.specularGlossinessTexture.index, filePath };
        if ( !tex.Info().IsBinary() ) {
            mat->m_specularGlossinessTexture    = tex.Info().FileName;
            mat->m_hasSpecularGlossinessTexture = true;
        }
        else { LOG( logINFO ) << "GLTF converter -- Embeded texture not supported yet"; }
        // get sampler information for this texture
        int samplerIndex = doc.textures[specularGloss.specularGlossinessTexture.index].sampler;
        if ( samplerIndex >= 0 ) {
            mat->m_specularGlossinessSampler = convertSampler( doc.samplers[samplerIndex] );
        }
        getMaterialTextureTransform( specularGloss.specularGlossinessTexture.extensionsAndExtras,
                                     mat->m_specularGlossinessTransform );
    }

    getMaterialExtensions(
        doc, filePath, meshMaterial, mat, { "KHR_materials_pbrSpecularGlossiness" } );
    return mat;
}

Ra::Core::Asset::MaterialData* buildMaterial( const gltf::Document& doc,
                                              int32_t meshIndex,
                                              const std::string& filePath,
                                              int32_t meshPartNumber,
                                              const MaterialData& meshMaterial ) {
#ifdef LEGACY_IMPLEMENTATION
    if ( meshMaterial.isMetallicRoughness() ) {
        return buildMetallicRoughnessMaterial(
            doc, meshIndex, filePath, meshPartNumber, meshMaterial );
    }
    else {
        // Check if extension "KHR_materials_pbrSpecularGlossiness" is available
        auto extensionsAndExtras = meshMaterial.Data().extensionsAndExtras;
        if ( !extensionsAndExtras.empty() ) {
            auto extensions = extensionsAndExtras.find( "extensions" );
            if ( extensions != extensionsAndExtras.end() ) {
                auto iter = extensions->find( "KHR_materials_pbrSpecularGlossiness" );
                if ( iter != extensions->end() ) {
                    gltf_PBRSpecularGlossiness gltfMaterial;
                    from_json( *iter, gltfMaterial );
                    return buildSpecularGlossinessMaterial(
                        doc, meshIndex, filePath, meshPartNumber, meshMaterial, gltfMaterial );
                }
            }
        }
        /// TODO : generate a default MetallicRoughness with the base parameters
        return buildDefaultMaterial( doc, meshIndex, filePath, meshPartNumber, meshMaterial );
    }
#else
    if ( meshMaterial.isSpecularGlossiness() ) {
        auto extensions = meshMaterial.Data().extensionsAndExtras.find( "extensions" );
        auto iter       = extensions->find( "KHR_materials_pbrSpecularGlossiness" );
        gltf_PBRSpecularGlossiness gltfMaterial;
        from_json( *iter, gltfMaterial );
        return buildSpecularGlossinessMaterial(
            doc, meshIndex, filePath, meshPartNumber, meshMaterial, gltfMaterial );
    }
    else {
        if ( meshMaterial.hasData() ) {
            return buildMetallicRoughnessMaterial(
                doc, meshIndex, filePath, meshPartNumber, meshMaterial );
        }
        else {
            /// generate a default MetallicRoughness with the base parameters
            return buildDefaultMaterial( doc, meshIndex, filePath, meshPartNumber, meshMaterial );
        }
    }
#endif
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
