#include "nlohmann/json.hpp"
#include <Core/Resources/Resources.hpp>
#include <Engine/Data/GLTFMaterial.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/RadiumEngine.hpp>

#include <Engine/Data/MetallicRoughnessMaterial.hpp>
#include <Engine/Data/SpecularGlossinessMaterial.hpp>

#include <Engine/Data/Texture.hpp>
#include <globjects/Texture.h>

#include <fstream>

namespace Ra {
namespace Engine {
namespace Data {

bool GLTFMaterial::s_bsdfLutsLoaded { false };
Ra::Engine::Data::Texture* GLTFMaterial::s_ggxlut { nullptr };
Ra::Engine::Data::Texture* GLTFMaterial::s_sheenElut { nullptr };
Ra::Engine::Data::Texture* GLTFMaterial::s_charlielut { nullptr };

std::string GLTFMaterial::s_shaderBasePath {};
nlohmann::json GLTFMaterial::m_parametersMetadata = {};

std::shared_ptr<GLTFMaterial::GltfAlphaModeEnumConverter>
    GLTFMaterial::s_AlphaModeEnum( new GLTFMaterial::GltfAlphaModeEnumConverter(
        { { Core::Material::AlphaMode::Opaque, "Opaque" },
          { Core::Material::AlphaMode::Mask, "Mask" },
          { Core::Material::AlphaMode::Blend, "Blend" } } ) );

GLTFMaterial::GLTFMaterial( const std::string& name, const std::string& materialName ) :
    Material( name, materialName, Material::MaterialAspect::MAT_OPAQUE ) {
    auto& renderParameters = Data::ShaderParameterProvider::getParameters();
    renderParameters.addEnumConverter( "material.baseMaterial.alphaMode", s_AlphaModeEnum );
}

GLTFMaterial::~GLTFMaterial() {
    this->m_textures.clear();
}

const Core::Material::GLTFTextureTransform*
GLTFMaterial::getTextureTransform( const TextureSemantic& semantic ) const {
    if ( semantic == "TEX_NORMAL" ) { return m_normalTextureTransform.get(); }
    if ( semantic == "TEX_EMISSIVE" ) { return m_emissiveTextureTransform.get(); }
    if ( semantic == "TEX_OCCLUSION" ) { return m_occlusionTextureTransform.get(); }
    return nullptr;
}

void GLTFMaterial::updateGL() {
    m_isOpenGlConfigured = true;
    // Load textures
    auto texManager        = RadiumEngine::getInstance()->getTextureManager();
    auto& renderParameters = getParameters();
    if ( GLTFMaterial::s_ggxlut == nullptr ) {
        Ra::Engine::Data::TextureParameters ggxLut;
        ggxLut.name            = "GLTFMaterial::ggxLut";
        GLTFMaterial::s_ggxlut = texManager->getOrLoadTexture( ggxLut, false );
    }
    renderParameters.addParameter( "material.baseMaterial.ggxLut", GLTFMaterial::s_ggxlut );

    for ( const auto& tex : m_pendingTextures ) {
        // only manage GLTFMaterial texture semantic.
        if ( tex.first == "TEX_EMISSIVE" ) {
            m_textures[tex.first] = texManager->getOrLoadTexture( tex.second, true );
        }
        else if ( tex.first == "TEX_NORMAL" || tex.first == "TEX_OCCLUSION" ) {
            m_textures[tex.first] = texManager->getOrLoadTexture( tex.second, false );
        }
    }

    m_pendingTextures.erase( { "TEX_EMISSIVE" } );
    m_pendingTextures.erase( { "TEX_NORMAL" } );
    m_pendingTextures.erase( { "TEX_OCCLUSION" } );

    renderParameters.addParameter( "material.baseMaterial.emissiveFactor", m_emissiveFactor );
    renderParameters.addParameter( "material.baseMaterial.alphaMode", m_alphaMode );
    renderParameters.addParameter( "material.baseMaterial.alphaCutoff", m_alphaCutoff );
    renderParameters.addParameter( "material.baseMaterial.doubleSided", m_doubleSided );
    renderParameters.addParameter( "material.baseMaterial.ior", m_indexOfRefraction );

    auto tex = getTexture( { "TEX_NORMAL" } );
    if ( tex != nullptr ) {
        renderParameters.addParameter( "material.baseMaterial.normalTextureScale",
                                       m_normalTextureScale );
        renderParameters.addParameter( "material.baseMaterial.normal", tex );
        if ( m_normalTextureTransform ) {
            auto tr = m_normalTextureTransform->getTransformationAsMatrix();
            renderParameters.addParameter( "material.baseMaterial.normalTransform", tr );
        }
    }

    tex = getTexture( { "TEX_OCCLUSION" } );
    if ( tex != nullptr ) {
        renderParameters.addParameter( "material.baseMaterial.occlusionStrength",
                                       m_occlusionStrength );
        renderParameters.addParameter( "material.baseMaterial.occlusion", tex );
        if ( m_occlusionTextureTransform ) {
            auto tr = m_occlusionTextureTransform->getTransformationAsMatrix();
            renderParameters.addParameter( "material.baseMaterial.occlusionTransform", tr );
        }
    }

    tex = getTexture( { "TEX_EMISSIVE" } );
    if ( tex != nullptr ) {
        renderParameters.addParameter( "material.baseMaterial.emissive", tex );
        if ( m_emissiveTextureTransform ) {
            auto tr = m_emissiveTextureTransform->getTransformationAsMatrix();
            renderParameters.addParameter( "material.baseMaterial.emmissiveTransform", tr );
        }
    }

    for ( const auto& l : m_layers ) {
        l->updateGL();
    }
}

void GLTFMaterial::updateFromParameters() {
    if ( m_isOpenGlConfigured ) {
        auto& renderParameters = getParameters();
        m_emissiveFactor       = renderParameters.getParameter<Ra::Core::Utils::Color>(
            "material.baseMaterial.emissiveFactor" );
        m_alphaMode = renderParameters.getParameter<Core::Material::AlphaMode>(
            "material.baseMaterial.alphaMode" );
        m_alphaCutoff =
            renderParameters.getParameter<Scalar>( "material.baseMaterial.alphaCutoff" );
        m_doubleSided = renderParameters.getParameter<bool>( "material.baseMaterial.doubleSided" );
        m_indexOfRefraction = renderParameters.getParameter<Scalar>( "material.baseMaterial.ior" );

        for ( const auto& l : m_layers ) {
            l->updateFromParameters();
        }
    }
}

bool GLTFMaterial::isTransparent() const {
    return m_alphaMode == 2 || std::any_of( m_layers.begin(), m_layers.end(), []( const auto& l ) {
               return l->isTransparent();
           } );
}

void GLTFMaterial::registerMaterial() {
    // gets the resource path
    auto resourcesRootDir { RadiumEngine::getInstance()->getResourcesDir() };

    s_shaderBasePath          = resourcesRootDir + "Shaders/Materials/GLTF";
    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();
    shaderProgramManager->addNamedString( { "/baseGLTFMaterial.glsl" },
                                          s_shaderBasePath + "/Materials/baseGLTFMaterial.glsl" );

    if ( !GLTFMaterial::s_bsdfLutsLoaded ) {
        auto* engine  = Ra::Engine::RadiumEngine::getInstance();
        auto* texMngr = engine->getTextureManager();
        auto& ggxLut  = texMngr->addTexture( "GLTFMaterial::ggxLut", 1024, 1024, nullptr );
        // load the texture image without OpenGL initialization
        ggxLut.name = s_shaderBasePath + "/BSDF_LUTs/lut_ggx.png";
        texMngr->loadTextureImage( ggxLut );
        // Set the registered name again for further access to the texture
        ggxLut.name = "GLTFMaterial::ggxLut";

        auto& sheenELut = texMngr->addTexture( "GLTFMaterial::sheenELut", 1024, 1024, nullptr );
        sheenELut.name  = s_shaderBasePath + "/BSDF_LUTs/lut_sheen_E.png";
        texMngr->loadTextureImage( sheenELut );
        // Set the registered name again for further access to the texture
        sheenELut.name = "GLTFMaterial::sheenELut";

        auto& charlieLut = texMngr->addTexture( "GLTFMaterial::charlieLut", 1024, 1024, nullptr );
        charlieLut.name  = s_shaderBasePath + "/BSDF_LUTs/lut_charlie.png";
        texMngr->loadTextureImage( charlieLut );
        // Set the registered name again for further access to the texture
        charlieLut.name = "GLTFMaterial::charlieLut";

        GLTFMaterial::s_bsdfLutsLoaded = true;
    }
    // Registering parameters metadata
    std::ifstream metadata( s_shaderBasePath + "/Metadata/GlTFMaterial.json" );
    metadata >> m_parametersMetadata;

    MetallicRoughness::registerMaterial();
    SpecularGlossiness::registerMaterial();
}

void GLTFMaterial::unregisterMaterial() {
    MetallicRoughness::unregisterMaterial();
    SpecularGlossiness::unregisterMaterial();
}

std::map<std::string,
         std::function<std::unique_ptr<GLTFMaterialExtension>( GLTFMaterial& baseMaterial,
                                                               const std::string& instanceName,
                                                               const void* source )>>
    extensionBuilder = {
        { "KHR_materials_ior",
          []( GLTFMaterial& baseMaterial,
              const std::string& /*instanceName*/,
              const auto* source ) {
              auto iorProvider = reinterpret_cast<const Core::Material::GLTFIor*>( source );
              // Not a layer, set a property on the material
              baseMaterial.setIndexOfRefraction( iorProvider->m_ior );
              return nullptr;
          } },
        { "KHR_materials_unlit",
          []( GLTFMaterial& baseMaterial,
              const std::string& /*instanceName*/,
              const auto* source ) {
              auto unlitProvider = reinterpret_cast<const Core::Material::GLTFUnlit*>( source );
              // Not a layer, set a property on the material
              baseMaterial.setUnlitStatus( unlitProvider->active );
              return nullptr;
          } },
        { "KHR_materials_clearcoat",
          []( GLTFMaterial& baseMaterial, const std::string& instanceName, const auto* source ) {
              return std::make_unique<GLTFClearcoat>(
                  baseMaterial,
                  instanceName,
                  reinterpret_cast<const Core::Material::GLTFClearcoatLayer*>( source ) );
          } },
        { "KHR_materials_specular",
          []( GLTFMaterial& baseMaterial, const std::string& instanceName, const auto* source ) {
              return std::make_unique<GLTFSpecular>(
                  baseMaterial,
                  instanceName,
                  reinterpret_cast<const Core::Material::GLTFSpecularLayer*>( source ) );
          } },
        { "KHR_materials_sheen",
          []( GLTFMaterial& baseMaterial, const std::string& instanceName, const auto* source ) {
              return std::make_unique<GLTFSheen>(
                  baseMaterial,
                  instanceName,
                  reinterpret_cast<const Core::Material::GLTFSheenLayer*>( source ) );
          } } };

void GLTFMaterial::fillBaseFrom( const Core::Material::BaseGLTFMaterial* source ) {

    // Warning, must modify this if textures are embedded in the GLTF file
    if ( source->m_hasNormalTexture ) {
        addTexture( { "TEX_NORMAL" }, source->m_normalTexture, source->m_normalSampler );
        m_normalTextureTransform = std::move( source->m_normalTextureTransform );
    }
    if ( source->m_hasOcclusionTexture ) {
        addTexture( { "TEX_OCCLUSION" }, source->m_occlusionTexture, source->m_occlusionSampler );
        m_occlusionTextureTransform = std::move( source->m_occlusionTextureTransform );
    }
    if ( source->m_hasEmissiveTexture ) {
        addTexture( { "TEX_EMISSIVE" }, source->m_emissiveTexture, source->m_emissiveSampler );
        m_emissiveTextureTransform = std::move( source->m_emissiveTextureTransform );
    }
    m_normalTextureScale = source->m_normalTextureScale;
    m_occlusionStrength  = source->m_occlusionStrength;
    m_emissiveFactor     = source->m_emissiveFactor;
    m_alphaMode          = source->m_alphaMode;
    m_alphaCutoff        = source->m_alphaCutoff;
    m_doubleSided        = source->m_doubleSided;

    for ( const auto& ext : source->m_extensions ) {
        auto it = extensionBuilder.find( ext.first );
        if ( it != extensionBuilder.end() ) {
            auto e = it->second( *this, ext.second->getName(), ext.second.get() );
            if ( e ) { m_layers.emplace_back( std::move( e ) ); }
        }
        else {
            LOG( Ra::Core::Utils::logERROR )
                << "Unable to find translator for gltf extension " << ext.first << "!!";
        }
    }
}

std::list<std::string> GLTFMaterial::getPropertyList() const {
    std::list<std::string> props = Ra::Engine::Data::Material::getPropertyList();
    // Expose the new GLTF__INTERFACE that will eveolve until it is submitted to Radium
    // GLSL/Material interface
    props.emplace_back( "GLTF_MATERIAL_INTERFACE" );
    // unlit
    if ( getUnlitStatus() ) { props.emplace_back( "MATERIAL_UNLIT" ); }
    // textures
    if ( m_pendingTextures.find( { "TEX_NORMAL" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_NORMAL" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_NORMAL" );
        if ( m_normalTextureTransform ) { props.emplace_back( "TEXTURE_COORD_TRANSFORM_NORMAL" ); }
    }
    if ( m_pendingTextures.find( { "TEX_OCCLUSION" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_OCCLUSION" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_OCCLUSION" );
        if ( m_occlusionTextureTransform ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_OCCLUSION" );
        }
    }
    if ( m_pendingTextures.find( { "TEX_EMISSIVE" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_EMISSIVE" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_EMISSIVE" );
        if ( m_emissiveTextureTransform ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_EMISSIVE" );
        }
    }

    for ( const auto& l : m_layers ) {
        props.splice( props.end(), l->getPropertyList() );
    }
    return props;
}

nlohmann::json GLTFMaterial::getParametersMetadata() const {
    return m_parametersMetadata;
}

/* --- clearcoat layer --- */

GLTFClearcoat::GLTFClearcoat( GLTFMaterial& baseMaterial,
                              const std::string& instanceName,
                              const Core::Material::GLTFClearcoatLayer* source ) :
    GLTFMaterialExtension( baseMaterial, instanceName, "GLTF_ClearcoatLayer" ) {

    // Warning, must modify this if textures are embedded in the GLTF file

    m_clearcoatFactor = source->m_clearcoatFactor;
    if ( source->m_hasClearcoatTexture ) {

        addTexture( { "TEX_CLEARCOAT" }, source->m_clearcoatTexture, source->m_clearcoatSampler );
        if ( source->m_clearcoatTextureTransform ) {
            m_textureTransform["TEX_CLEARCOAT"] = std::move( source->m_clearcoatTextureTransform );
        }
    }

    m_clearcoatRoughnessFactor = source->m_clearcoatRoughnessFactor;
    if ( source->m_hasClearcoatRoughnessTexture ) {
        addTexture( { "TEX_CLEARCOATROUGHNESS" },
                    source->m_clearcoatRoughnessTexture,
                    source->m_clearcoatRoughnessSampler );
        if ( source->m_clearcoatRoughnessTextureTransform ) {
            m_textureTransform["TEX_CLEARCOATROUGHNESS"] =
                std::move( source->m_clearcoatRoughnessTextureTransform );
        }
    }

    if ( source->m_hasClearcoatNormalTexture ) {
        addTexture( { "TEX_CLEARCOATNORMAL" },
                    source->m_clearcoatNormalTexture,
                    source->m_clearcoatNormalSampler );
        m_clearcoatNormalTextureScale = source->m_clearcoatNormalTextureScale;
        if ( source->m_clearcoatNormalTextureTransform ) {
            m_textureTransform["TEX_CLEARCOATNORMAL"] =
                std::move( source->m_clearcoatNormalTextureTransform );
        }
    }
}

std::list<std::string> GLTFClearcoat::getPropertyList() const {
    std::list<std::string> props;
    props.emplace_back( "CLEARCOAT_LAYER" );
    // textures
    if ( m_pendingTextures.find( { "TEX_CLEARCOAT" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_CLEARCOAT" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_CLEARCOAT" );
        if ( m_textureTransform.find( "TEX_CLEARCOAT" ) != m_textureTransform.end() ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_CLEARCOAT" );
        }
    }
    if ( m_pendingTextures.find( { "TEX_CLEARCOATROUGHNESS" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_CLEARCOATROUGHNESS" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_CLEARCOATROUGHNESS" );
        if ( m_textureTransform.find( "TEX_CLEARCOATROUGHNESS" ) != m_textureTransform.end() ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_CLEARCOATROUGHNESS" );
        }
    }
    if ( m_pendingTextures.find( { "TEX_CLEARCOATNORMAL" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_CLEARCOATNORMAL" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_CLEARCOATNORMAL" );
        if ( m_textureTransform.find( "TEX_CLEARCOATNORMAL" ) != m_textureTransform.end() ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_CLEARCOATNORMAL" );
        }
    }
    return props;
}

void GLTFClearcoat::updateGL() {
    m_baseMaterial.getParameters().addParameter( "material.baseMaterial.clearcoat.clearcoatFactor",
                                                 m_clearcoatFactor );
    m_baseMaterial.getParameters().addParameter(
        "material.baseMaterial.clearcoat.clearcoatRoughnessFactor", m_clearcoatRoughnessFactor );

    // Load textures
    if ( !m_pendingTextures.empty() ) {
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        for ( const auto& tex : m_pendingTextures ) {
            // According to the clearcoat spec, all clearcoat textures are in RGB linear space
            // https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_clearcoat
            m_textures[tex.first] = texManager->getOrLoadTexture( tex.second, false );
        }
        m_pendingTextures.clear();
    }
    Ra::Engine::Data::Texture* tex = getTexture( { "TEX_CLEARCOAT" } );
    if ( tex != nullptr ) {
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.clearcoat.clearcoatTexture", tex );
        auto it = m_textureTransform.find( "TEX_CLEARCOAT" );
        if ( it != m_textureTransform.end() ) {
            auto tr = it->second->getTransformationAsMatrix();
            m_baseMaterial.getParameters().addParameter(
                "material.baseMaterial.clearcoat.clearcoatTextureTransform", tr );
        }
    }

    tex = getTexture( { "TEX_CLEARCOATROUGHNESS" } );
    if ( tex != nullptr ) {
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.clearcoat.clearcoatRoughnessTexture", tex );
        auto it = m_textureTransform.find( "TEX_CLEARCOATROUGHNESS" );
        if ( it != m_textureTransform.end() ) {
            auto tr = it->second->getTransformationAsMatrix();
            m_baseMaterial.getParameters().addParameter(
                "material.baseMaterial.clearcoat.clearcoatRoughnessTextureTransform", tr );
        }
    }

    tex = getTexture( { "TEX_CLEARCOATNORMAL" } );
    if ( tex != nullptr ) {
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.clearcoat.clearcoatNormalTextureScale",
            m_clearcoatNormalTextureScale );
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.clearcoat.clearcoatNormalTexture", tex );
        auto it = m_textureTransform.find( "TEX_CLEARCOATNORMAL" );
        if ( it != m_textureTransform.end() ) {
            auto tr = it->second->getTransformationAsMatrix();
            m_baseMaterial.getParameters().addParameter(
                "material.baseMaterial.clearcoat.clearcoatNormalTextureTransform", tr );
        }
    }
}

void GLTFClearcoat::updateFromParameters() {
    m_clearcoatFactor = m_baseMaterial.getParameters().getParameter<Scalar>(
        "material.baseMaterial.clearcoat.clearcoatFactor" );
    m_clearcoatRoughnessFactor = m_baseMaterial.getParameters().getParameter<Scalar>(
        "material.baseMaterial.clearcoat.clearcoatRoughnessFactor" );
}

/* --- specular layer --- */

GLTFSpecular::GLTFSpecular( GLTFMaterial& baseMaterial,
                            const std::string& instanceName,
                            const Core::Material::GLTFSpecularLayer* source ) :
    GLTFMaterialExtension( baseMaterial, instanceName, "GLTF_ClearcoatLayer" ) {

    // Warning, must modify this if textures are embedded in the GLTF file

    m_specularFactor = source->m_specularFactor;
    if ( source->m_hasSpecularTexture ) {

        addTexture(
            { "TEXTURE_SPECULAR_EXT" }, source->m_specularTexture, source->m_specularSampler );
        if ( source->m_specularTextureTransform ) {
            m_textureTransform["TEXTURE_SPECULAR_EXT"] =
                std::move( source->m_specularTextureTransform );
        }
    }

    m_specularColorFactor = source->m_specularColorFactor;
    if ( source->m_hasSpecularColorTexture ) {
        addTexture( { "TEXTURE_SPECULARCOLOR_EXT" },
                    source->m_specularColorTexture,
                    source->m_specularColorSampler );
        if ( source->m_specularColorTextureTransform ) {
            m_textureTransform["TEXTURE_SPECULARCOLOR_EXT"] =
                std::move( source->m_specularColorTextureTransform );
        }
    }
}

std::list<std::string> GLTFSpecular::getPropertyList() const {
    std::list<std::string> props;
    props.emplace_back( "SPECULAR_LAYER" );
    // textures
    if ( m_pendingTextures.find( { "TEXTURE_SPECULAR_EXT" } ) != m_pendingTextures.end() ||
         getTexture( { "TEXTURE_SPECULAR_EXT" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_SPECULAR_EXT" );
        if ( m_textureTransform.find( "TEXTURE_SPECULAR_EXT" ) != m_textureTransform.end() ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_SPECULAR_EXT" );
        }
    }
    if ( m_pendingTextures.find( { "TEXTURE_SPECULARCOLOR_EXT" } ) != m_pendingTextures.end() ||
         getTexture( { "TEXTURE_SPECULARCOLOR_EXT" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_SPECULARCOLOR_EXT" );
        if ( m_textureTransform.find( "TEXTURE_SPECULARCOLOR_EXT" ) != m_textureTransform.end() ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_SPECULARCOLOR_EXT" );
        }
    }
    return props;
}

void GLTFSpecular::updateGL() {
    m_baseMaterial.getParameters().addParameter( "material.baseMaterial.specular.specularFactor",
                                                 m_specularFactor );
    m_baseMaterial.getParameters().addParameter(
        "material.baseMaterial.specular.specularColorFactor", m_specularColorFactor );

    // Load textures
    if ( !m_pendingTextures.empty() ) {
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        for ( const auto& tex : m_pendingTextures ) {
            // textures are in sRGB, must be linearized
            m_textures[tex.first] = texManager->getOrLoadTexture( tex.second, true );
        }
        m_pendingTextures.clear();
    }
    auto tex = getTexture( { "TEXTURE_SPECULAR_EXT" } );
    if ( tex != nullptr ) {
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.specular.specularTexture", tex );
        auto it = m_textureTransform.find( "TEXTURE_SPECULAR_EXT" );
        if ( it != m_textureTransform.end() ) {
            auto tr = it->second->getTransformationAsMatrix();
            m_baseMaterial.getParameters().addParameter(
                "material.baseMaterial.specular.specularTextureTransform", tr );
        }
    }

    tex = getTexture( { "TEXTURE_SPECULARCOLOR_EXT" } );
    if ( tex != nullptr ) {
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.specular.specularColorTexture", tex );
        auto it = m_textureTransform.find( "TEXTURE_SPECULARCOLOR_EXT" );
        if ( it != m_textureTransform.end() ) {
            auto tr = it->second->getTransformationAsMatrix();
            m_baseMaterial.getParameters().addParameter(
                "material.baseMaterial.specular.specularColorTextureTransform", tr );
        }
    }
}

void GLTFSpecular::updateFromParameters() {
    m_specularFactor = m_baseMaterial.getParameters().getParameter<Scalar>(
        "material.baseMaterial.specular.specularFactor" );
    m_specularColorFactor = m_baseMaterial.getParameters().getParameter<Ra::Core::Utils::Color>(
        "material.baseMaterial.specular.specularColorFactor" );
}

/* --- sheen layer --- */

GLTFSheen::GLTFSheen( GLTFMaterial& baseMaterial,
                      const std::string& instanceName,
                      const Core::Material::GLTFSheenLayer* source ) :
    GLTFMaterialExtension( baseMaterial, instanceName, "GLTF_SheenLayer" ) {

    // Warning, must modify this if textures are embedded in the GLTF file

    m_sheenRoughnessFactor = source->m_sheenRoughnessFactor;
    if ( source->m_hasSheenRoughnessTexture ) {

        addTexture( { "TEXTURE_SHEEN_ROUGHNESS" },
                    source->m_sheenRoughnessTexture,
                    source->m_sheenRoughnessTextureSampler );
        if ( source->m_sheenRoughnessTextureTransform ) {
            m_textureTransform["TEXTURE_SHEEN_ROUGHNESS"] =
                std::move( source->m_sheenRoughnessTextureTransform );
        }
    }

    m_sheenColorFactor = source->m_sheenColorFactor;
    if ( source->m_hasSheenColorTexture ) {
        addTexture( { "TEXTURE_SHEEN_COLOR" },
                    source->m_sheenColorTexture,
                    source->m_sheenColorTextureSampler );
        if ( source->m_sheenColorTextureTransform ) {
            m_textureTransform["TEXTURE_SHEEN_COLOR"] =
                std::move( source->m_sheenColorTextureTransform );
        }
    }
}

std::list<std::string> GLTFSheen::getPropertyList() const {
    std::list<std::string> props;
    props.emplace_back( "SHEEN_LAYER" );
    // textures
    if ( m_pendingTextures.find( { "TEXTURE_SHEEN_COLOR" } ) != m_pendingTextures.end() ||
         getTexture( { "TEXTURE_SHEEN_COLOR" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_SHEEN_COLOR" );
        if ( m_textureTransform.find( "TEXTURE_SHEEN_COLOR" ) != m_textureTransform.end() ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_SHEEN_COLOR" );
        }
    }
    if ( m_pendingTextures.find( { "TEXTURE_SHEEN_ROUGHNESS" } ) != m_pendingTextures.end() ||
         getTexture( { "TEXTURE_SHEEN_ROUGHNESS" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_SHEEN_ROUGHNESS" );
        if ( m_textureTransform.find( "TEXTURE_SHEEN_ROUGHNESS" ) != m_textureTransform.end() ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_SHEEN_ROUGHNESS" );
        }
    }
    return props;
}

void GLTFSheen::updateGL() {
    if ( GLTFMaterial::s_sheenElut == nullptr ) {
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        Ra::Engine::Data::TextureParameters lut;
        lut.name                   = "GLTFMaterial::sheenELut";
        GLTFMaterial::s_sheenElut  = texManager->getOrLoadTexture( lut, false );
        lut.name                   = "GLTFMaterial::charlieLut";
        GLTFMaterial::s_charlielut = texManager->getOrLoadTexture( lut, false );
    }
    auto& renderParameters = getParameters();
    renderParameters.addParameter( "material.baseMaterial.sheen.sheenE_LUT",
                                   GLTFMaterial::s_sheenElut );
    renderParameters.addParameter( "material.baseMaterial.sheen.charlieLUT",
                                   GLTFMaterial::s_charlielut );

    m_baseMaterial.getParameters().addParameter( "material.baseMaterial.sheen.sheenColorFactor",
                                                 m_sheenColorFactor );
    m_baseMaterial.getParameters().addParameter( "material.baseMaterial.sheen.sheenRoughnessFactor",
                                                 m_sheenRoughnessFactor );

    // Load textures
    if ( !m_pendingTextures.empty() ) {
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        for ( const auto& tex : m_pendingTextures ) {
            // textures are in sRGB, must be linearized
            m_textures[tex.first] = texManager->getOrLoadTexture( tex.second, true );
        }
        m_pendingTextures.clear();
    }
    auto tex = getTexture( { "TEXTURE_SHEEN_COLOR" } );
    if ( tex != nullptr ) {
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.sheen.sheenColorTexture", tex );
        auto it = m_textureTransform.find( "TEXTURE_SHEEN_COLOR" );
        if ( it != m_textureTransform.end() ) {
            auto tr = it->second->getTransformationAsMatrix();
            m_baseMaterial.getParameters().addParameter(
                "material.baseMaterial.sheen.sheenColorTextureTransform", tr );
        }
    }

    tex = getTexture( { "TEXTURE_SHEEN_ROUGHNESS" } );
    if ( tex != nullptr ) {
        m_baseMaterial.getParameters().addParameter(
            "material.baseMaterial.sheen.sheenRoughnessTexture", tex );
        auto it = m_textureTransform.find( "TEXTURE_SHEEN_ROUGHNESS" );
        if ( it != m_textureTransform.end() ) {
            auto tr = it->second->getTransformationAsMatrix();
            m_baseMaterial.getParameters().addParameter(
                "material.baseMaterial.sheen.sheenRoughnessTextureTransform", tr );
        }
    }
}

void GLTFSheen::updateFromParameters() {
    m_sheenColorFactor = m_baseMaterial.getParameters().getParameter<Ra::Core::Utils::Color>(
        "material.baseMaterial.sheen.sheenColorFactor" );
    m_sheenRoughnessFactor = m_baseMaterial.getParameters().getParameter<Scalar>(
        "material.baseMaterial.sheen.sheenRoughnessFactor" );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
