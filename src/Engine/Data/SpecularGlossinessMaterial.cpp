#include <Core/Material/SpecularGlossinessMaterialData.hpp>
#include <Core/Resources/Resources.hpp>
#include <Engine/Data/MaterialConverters.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/Data/SpecularGlossinessMaterial.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>

namespace Ra {
namespace Engine {
namespace Data {

using namespace Ra::Engine::Rendering;

const std::string SpecularGlossiness::m_materialName { "SpecularGlossiness" };

SpecularGlossiness::SpecularGlossiness( const std::string& instanceName ) :
    GLTFMaterial( instanceName, m_materialName ) {}

SpecularGlossiness::~SpecularGlossiness() = default;

void SpecularGlossiness::updateGL() {
    if ( !m_isDirty ) { return; }
    // manage inherited pending textures
    GLTFMaterial::updateGL();
    // manage specific textures
    auto texManager = RadiumEngine::getInstance()->getTextureManager();
    for ( const auto& tex : m_pendingTextures ) {
        bool tolinear = ( tex.first == "TEX_DIFFUSE" || tex.first == "TEX_SPECULARGLOSSINESS" );
        m_textures[tex.first] = texManager->getOrLoadTexture( tex.second, tolinear );
    }

    m_pendingTextures.clear();
    m_isDirty              = false;
    auto& renderParameters = getParameters();
    renderParameters.addParameter( "material.diffuseFactor", m_diffuseFactor );
    renderParameters.addParameter( "material.specularFactor", m_specularFactor );
    renderParameters.addParameter( "material.glossinessFactor", m_glossinessFactor );

    auto tex = getTexture( { "TEX_DIFFUSE" } );
    if ( tex != nullptr ) {
        renderParameters.addParameter( "material.diffuse", tex );
        if ( m_diffuseTextureTransform ) {
            auto ct = std::cos( m_diffuseTextureTransform->rotation );
            auto st = std::sin( m_diffuseTextureTransform->rotation );
            Ra::Core::Matrix3 tr;
            tr << m_diffuseTextureTransform->scale[0] * ct,
                -m_diffuseTextureTransform->scale[1] * st, m_diffuseTextureTransform->offset[0],
                m_diffuseTextureTransform->scale[0] * st, m_diffuseTextureTransform->scale[1] * ct,
                m_diffuseTextureTransform->offset[1], 0, 0, 1;
            renderParameters.addParameter( "material.diffuseTransform", tr );
        }
    }

    tex = getTexture( { "TEX_SPECULARGLOSSINESS" } );
    if ( tex != nullptr ) {
        renderParameters.addParameter( "material.specularGlossiness", tex );
        if ( m_specularGlossinessTransform ) {
            auto ct = std::cos( m_specularGlossinessTransform->rotation );
            auto st = std::sin( m_specularGlossinessTransform->rotation );
            Ra::Core::Matrix3 tr;
            tr << m_specularGlossinessTransform->scale[0] * ct,
                -m_specularGlossinessTransform->scale[1] * st,
                m_specularGlossinessTransform->offset[0],
                m_specularGlossinessTransform->scale[0] * st,
                m_specularGlossinessTransform->scale[1] * ct,
                m_specularGlossinessTransform->offset[1], 0, 0, 1;
            renderParameters.addParameter( "material.specularGlossinessTransform", tr );
        }
    }
}

void SpecularGlossiness::updateFromParameters() {
    GLTFMaterial::updateFromParameters();
    if ( m_isOpenGlConfigured ) {
        auto& renderParameters = getParameters();
        m_diffuseFactor =
            renderParameters.getParameter<Ra::Core::Utils::Color>( "material.diffuseFactor" );
        m_specularFactor =
            renderParameters.getParameter<Ra::Core::Utils::Color>( "material.specularFactor" );
        m_glossinessFactor = renderParameters.getParameter<Scalar>( "material.glossinessFactor" );
    }
}

void SpecularGlossiness::registerMaterial() {
    // gets the resource path of the plugins
    auto shaderPath = s_shaderBasePath;

    EngineMaterialConverters::registerMaterialConverter( m_materialName,
                                                         SpecularGlossinessMaterialConverter() );

    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();
    shaderProgramManager->addNamedString( { "/SpecularGlossiness.glsl" },
                                          shaderPath + "/Materials/SpecularGlossiness.glsl" );

    // registering re-usable shaders
    auto baseConfiguration =
        ShaderConfiguration { m_materialName,
                              shaderPath + "/Materials/baseGLTFMaterial.vert.glsl",
                              shaderPath + "/Materials/baseGLTFMaterial_LitOpaque.frag.glsl" };
    baseConfiguration.addInclude( { "\"SpecularGlossiness.glsl\"" },
                                  Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
    ShaderConfigurationFactory::addConfiguration( baseConfiguration );

    auto zprepassConfiguration =
        ShaderConfiguration { "ZPrepass" + m_materialName,
                              shaderPath + "/Materials/baseGLTFMaterial.vert.glsl",
                              shaderPath + "/Materials/baseGLTFMaterial_Zprepass.frag.glsl" };
    zprepassConfiguration.addInclude( { "\"SpecularGlossiness.glsl\"" },
                                      Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
    ShaderConfigurationFactory::addConfiguration( zprepassConfiguration );

    auto litoitConfiguration =
        ShaderConfiguration { "LitOIT" + m_materialName,
                              shaderPath + "/Materials/baseGLTFMaterial.vert.glsl",
                              shaderPath + "/Materials/baseGLTFMaterial_LitOIT.frag.glsl" };
    litoitConfiguration.addInclude( { "\"SpecularGlossiness.glsl\"" },
                                    Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
    ShaderConfigurationFactory::addConfiguration( litoitConfiguration );

    EngineRenderTechniques::registerDefaultTechnique(
        m_materialName,

        []( RenderTechnique& rt, bool isTransparent ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory) : BlinnPhong
            auto lpconfig = ShaderConfigurationFactory::getConfiguration( m_materialName );
            rt.setConfiguration( *lpconfig, DefaultRenderingPasses::LIGHTING_OPAQUE );

            // Z prepass (Reccomanded) : DepthAmbiantPass
            auto dpconfig =
                ShaderConfigurationFactory::getConfiguration( "ZPrepass" + m_materialName );
            rt.setConfiguration( *dpconfig, DefaultRenderingPasses::Z_PREPASS );
            // Uber is sometimes transparent ...
            // Transparent pass (Optional) : If Transparent ... add LitOIT
            if ( isTransparent ) {
                auto tpconfig =
                    ShaderConfigurationFactory::getConfiguration( "LitOIT" + m_materialName );
                rt.setConfiguration( *tpconfig, DefaultRenderingPasses::LIGHTING_TRANSPARENT );
            }
        } );
}

void SpecularGlossiness::unregisterMaterial() {
    // strange bug here, using m_materialName segfault at exit
    EngineRenderTechniques::removeDefaultTechnique( { "SpecularGlossiness" } );
    EngineMaterialConverters::removeMaterialConverter( { "SpecularGlossiness" } );
}

std::list<std::string> SpecularGlossiness::getPropertyList() const {
    std::list<std::string> props = GLTFMaterial::getPropertyList();
    if ( m_pendingTextures.find( { "TEX_DIFFUSE" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_DIFFUSE" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_DIFFUSE" );
        if ( m_diffuseTextureTransform ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_DIFFUSE" );
        }
    }
    if ( m_pendingTextures.find( { "TEX_SPECULARGLOSSINESS" } ) != m_pendingTextures.end() ||
         getTexture( { "TEX_SPECULARGLOSSINESS" } ) != nullptr ) {
        props.emplace_back( "TEXTURE_SPECULARGLOSSINESS" );
        if ( m_specularGlossinessTransform ) {
            props.emplace_back( "TEXTURE_COORD_TRANSFORM_SPECULARGLOSSINESS" );
        }
    }
    return props;
}

const Core::Material::GLTFTextureTransform*
SpecularGlossiness::getTextureTransform( const TextureSemantic& semantic ) const {
    if ( semantic == "TEX_DIFFUSE" ) { return m_diffuseTextureTransform.get(); }
    if ( semantic == "TEX_SPECULARGLOSSINESS" ) { return m_specularGlossinessTransform.get(); }
    return GLTFMaterial::getTextureTransform( semantic );
}

/*
 * Core to Engine converter
 */
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
