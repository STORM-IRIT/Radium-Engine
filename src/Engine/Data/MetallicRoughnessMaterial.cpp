#include <Core/Material/MetallicRoughnessMaterialData.hpp>
#include <Core/Resources/Resources.hpp>
#include <Engine/Data/GLTFMaterial.hpp>
#include <Engine/Data/MaterialConverters.hpp>
#include <Engine/Data/MetallicRoughnessMaterial.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>

namespace Ra {
namespace Engine {
namespace Data {

using namespace Ra::Engine::Rendering;

const std::string MetallicRoughness::m_materialName { "MetallicRoughness" };

MetallicRoughness::MetallicRoughness( const std::string& instanceName ) :
    GLTFMaterial( instanceName, m_materialName ) {}

MetallicRoughness::~MetallicRoughness() = default;

void MetallicRoughness::updateGL() {
    if ( !m_isDirty ) { return; }
    // manage inherited pending textures
    GLTFMaterial::updateGL();

    // manage specific textures
    auto texManager = RadiumEngine::getInstance()->getTextureManager();
    for ( const auto& tex : m_pendingTextures ) {
        bool tolinear         = ( tex.first == "TEX_BASECOLOR" );
        m_textures[tex.first] = texManager->getOrLoadTexture( tex.second, tolinear );
    }

    m_pendingTextures.clear();
    m_isDirty = false;

    auto& renderParameters = getParameters();
    renderParameters.addParameter( "material.baseColorFactor", m_baseColorFactor );
    renderParameters.addParameter( "material.metallicFactor", m_metallicFactor );
    renderParameters.addParameter( "material.roughnessFactor", m_roughnessFactor );

    auto tex = getTexture( { "TEX_BASECOLOR" } );
    if ( tex != nullptr ) {
        renderParameters.addParameter( "material.baseColor", tex );
        if ( m_baseTextureTransform ) {
            auto tr = m_baseTextureTransform->getTransformationAsMatrix();
            renderParameters.addParameter( "material.baseTransform", tr );
        }
    }

    tex = getTexture( { "TEX_METALLICROUGHNESS" } );
    if ( tex != nullptr ) {
        renderParameters.addParameter( "material.metallicRoughness", tex );
        if ( m_metallicRoughnessTextureTransform ) {
            auto tr = m_metallicRoughnessTextureTransform->getTransformationAsMatrix();
            renderParameters.addParameter( "material.metallicRoughnessTransform", tr );
        }
    }
}

void MetallicRoughness::updateFromParameters() {

    GLTFMaterial::updateFromParameters();
    if ( m_isOpenGlConfigured ) {
        auto& renderParameters = getParameters();
        m_baseColorFactor =
            renderParameters.getParameter<Ra::Core::Utils::Color>( "material.baseColorFactor" );
        m_metallicFactor  = renderParameters.getParameter<Scalar>( "material.metallicFactor" );
        m_roughnessFactor = renderParameters.getParameter<Scalar>( "material.roughnessFactor" );
    }
}

void MetallicRoughness::registerMaterial() {
    // gets the resource path of the plugins
    auto shaderPath = s_shaderBasePath;

    EngineMaterialConverters::registerMaterialConverter( m_materialName,
                                                         MetallicRoughnessMaterialConverter() );

    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();
    shaderProgramManager->addNamedString( { "/MetallicRoughness.glsl" },
                                          shaderPath + "/Materials/MetallicRoughness.glsl" );

    // registering re-usable shaders
    auto baseConfiguration =
        ShaderConfiguration { m_materialName,
                              shaderPath + "/Materials/baseGLTFMaterial.vert.glsl",
                              shaderPath + "/Materials/baseGLTFMaterial_LitOpaque.frag.glsl" };
    baseConfiguration.addInclude( { "\"MetallicRoughness.glsl\"" },
                                  Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
    ShaderConfigurationFactory::addConfiguration( baseConfiguration );

    auto zprepassConfiguration =
        ShaderConfiguration { "ZPrepass" + m_materialName,
                              shaderPath + "/Materials/baseGLTFMaterial.vert.glsl",
                              shaderPath + "/Materials/baseGLTFMaterial_Zprepass.frag.glsl" };
    zprepassConfiguration.addInclude( { "\"MetallicRoughness.glsl\"" },
                                      Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
    ShaderConfigurationFactory::addConfiguration( zprepassConfiguration );

    auto litoitConfiguration =
        ShaderConfiguration { "LitOIT" + m_materialName,
                              shaderPath + "/Materials/baseGLTFMaterial.vert.glsl",
                              shaderPath + "/Materials/baseGLTFMaterial_LitOIT.frag.glsl" };
    litoitConfiguration.addInclude( { "\"MetallicRoughness.glsl\"" },
                                    Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
    ShaderConfigurationFactory::addConfiguration( litoitConfiguration );

    EngineRenderTechniques::registerDefaultTechnique(
        m_materialName,

        []( RenderTechnique& rt, bool isTransparent ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory)
            auto lpconfig = ShaderConfigurationFactory::getConfiguration( m_materialName );
            rt.setConfiguration( *lpconfig, DefaultRenderingPasses::LIGHTING_OPAQUE );

            // Z prepass (Recomanded) : DepthAmbiantPass
            auto dpconfig =
                ShaderConfigurationFactory::getConfiguration( "ZPrepass" + m_materialName );
            rt.setConfiguration( *dpconfig, DefaultRenderingPasses::Z_PREPASS );
            // Transparent pass (Optional) : If Transparent ... add LitOIT
            if ( isTransparent ) {
                auto tpconfig =
                    ShaderConfigurationFactory::getConfiguration( "LitOIT" + m_materialName );
                rt.setConfiguration( *tpconfig, DefaultRenderingPasses::LIGHTING_TRANSPARENT );
            }
        } );
}

void MetallicRoughness::unregisterMaterial() {
    // strange bug here, using m_materialName segfault at exit
    EngineMaterialConverters::removeMaterialConverter( { "MetallicRoughness" } );
    EngineRenderTechniques::removeDefaultTechnique( { "MetallicRoughness" } );
}

std::list<std::string> MetallicRoughness::getPropertyList() const {
    std::list<std::string> props = GLTFMaterial::getPropertyList();
    if ( m_pendingTextures.find( { "TEX_BASECOLOR" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_BASECOLOR" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_BASECOLOR" );
        if ( m_baseTextureTransform ) { props.emplace_back( "TEXTURE_COORD_TRANSFORM_BASECOLOR" ); }
    }
    if ( m_pendingTextures.find( { "TEX_METALLICROUGHNESS" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_METALLICROUGHNESS" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_METALLICROUGHNESS" );
        if ( m_metallicRoughnessTextureTransform ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_METALLICROUGHNESS" );
        }
    }
    return props;
}

const Core::Material::GLTFTextureTransform*
MetallicRoughness::getTextureTransform( const TextureSemantic& semantic ) const {
    if ( semantic == "TEX_BASECOLOR" ) { return m_baseTextureTransform.get(); }
    if ( semantic == "TEX_METALLICROUGHNESS" ) { return m_metallicRoughnessTextureTransform.get(); }
    return GLTFMaterial::getTextureTransform( semantic );
}

/*
 * Core to Engine converter
 */
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
