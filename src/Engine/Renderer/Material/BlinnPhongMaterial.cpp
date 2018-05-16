#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Material/MaterialConverters.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <Core/File/BlinnPhongMaterialData.hpp>

namespace Ra {
namespace Engine {

BlinnPhongMaterial::BlinnPhongMaterial( const std::string& name ) :
    Material( name, Material::MaterialAspect::MAT_OPAQUE ),
    m_kd( 0.9, 0.9, 0.9, 1.0 ),
    m_ks( 0.0, 0.0, 0.0, 1.0 ),
    m_ns( 1.0 ),
    m_alpha( 1.0 ) {}

BlinnPhongMaterial::~BlinnPhongMaterial() {
    m_textures.clear();
}

void BlinnPhongMaterial::updateGL() {
    if ( !m_isDirty )
    {
        return;
    }

    // Load textures
    TextureManager* texManager = TextureManager::getInstance();
    for ( const auto& tex : m_pendingTextures )
    {
        addTexture( tex.first, texManager->getOrLoadTexture( tex.second ) );
    }

    m_pendingTextures.clear();
    m_isDirty = false;
}

const std::string BlinnPhongMaterial::getShaderInclude() const {
    return "BlinnPhong";
}

void BlinnPhongMaterial::bind( const ShaderProgram* shader ) {
    shader->setUniform( "material.kd", m_kd );
    shader->setUniform( "material.ks", m_ks );
    shader->setUniform( "material.ns", m_ns );
    shader->setUniform( "material.alpha", m_alpha );

    Texture* tex = nullptr;
    uint texUnit = 0;

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_DIFFUSE );
    if ( tex != nullptr )
    {
        tex->bind( texUnit );
        shader->setUniform( "material.tex.kd", tex, texUnit );
        shader->setUniform( "material.tex.hasKd", 1 );
        ++texUnit;
    } else
    { shader->setUniform( "material.tex.hasKd", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_SPECULAR );
    if ( tex != nullptr )
    {
        tex->bind( texUnit );
        shader->setUniform( "material.tex.ks", tex, texUnit );
        shader->setUniform( "material.tex.hasKs", 1 );
        ++texUnit;
    } else
    { shader->setUniform( "material.tex.hasKs", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_NORMAL );
    if ( tex != nullptr )
    {
        tex->bind( texUnit );
        shader->setUniform( "material.tex.normal", tex, texUnit );
        shader->setUniform( "material.tex.hasNormal", 1 );
        ++texUnit;
    } else
    { shader->setUniform( "material.tex.hasNormal", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_SHININESS );
    if ( tex != nullptr )
    {
        tex->bind( texUnit );
        shader->setUniform( "material.tex.ns", tex, texUnit );
        shader->setUniform( "material.tex.hasNs", 1 );
        ++texUnit;
    } else
    { shader->setUniform( "material.tex.hasNs", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_ALPHA );
    if ( tex != nullptr )
    {
        tex->bind( texUnit );
        shader->setUniform( "material.tex.alpha", tex, texUnit );
        shader->setUniform( "material.tex.hasAlpha", 1 );
        ++texUnit;
    } else
    { shader->setUniform( "material.tex.hasAlpha", 0 ); }
}

bool BlinnPhongMaterial::isTransparent() const {
    return ( m_alpha < 1.0 ) || Material::isTransparent();
}


void BlinnPhongMaterial::registerMaterial() {
    // Defining Converter
    EngineMaterialConverters::registerMaterialConverter( "BlinnPhong",
                                                         BlinnPhongMaterialConverter() );
    //registering re-usable shaders
    Ra::Engine::ShaderConfiguration lpconfig(
        "BlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
        "Shaders/Materials/BlinnPhong/BlinnPhong.frag.glsl" );
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( lpconfig );

    // Registering technique
    Ra::Engine::EngineRenderTechniques::registerDefaultTechnique(
        "BlinnPhong",

        []( Ra::Engine::RenderTechnique& rt, bool isTransparent ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory) : BlinnPhong
            auto lpconfig = Ra::Engine::ShaderConfigurationFactory::getConfiguration( "BlinnPhong" );
            rt.setConfiguration( lpconfig, Ra::Engine::RenderTechnique::LIGHTING_OPAQUE );

            // Z prepass (Recommended) : DepthAmbiantPass
            Ra::Engine::ShaderConfiguration dpconfig(
                "DepthAmbiantBlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                "Shaders/Materials/BlinnPhong/DepthAmbientBlinnPhong.frag.glsl" );
            Ra::Engine::ShaderConfigurationFactory::addConfiguration( dpconfig );
            rt.setConfiguration( dpconfig, Ra::Engine::RenderTechnique::Z_PREPASS );
            // Transparent pass (0ptional) : If Transparent ... add LitOIT
            if ( isTransparent )
            {
                Ra::Engine::ShaderConfiguration tpconfig(
                    "LitOITBlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                    "Shaders/Materials/BlinnPhong/LitOITBlinnPhong.frag.glsl" );
                Ra::Engine::ShaderConfigurationFactory::addConfiguration( tpconfig );
                rt.setConfiguration( tpconfig, Ra::Engine::RenderTechnique::LIGHTING_TRANSPARENT );
            }
        } );
}

void BlinnPhongMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( "BlinnPhong" );
    EngineRenderTechniques::removeDefaultTechnique( "BlinnPhong" );
}

Material* BlinnPhongMaterialConverter::operator()( const Ra::Asset::MaterialData* toconvert ) {
    Ra::Engine::BlinnPhongMaterial* result =
      new Ra::Engine::BlinnPhongMaterial( toconvert->getName() );

    auto source = static_cast<const Ra::Asset::BlinnPhongMaterialData*>( toconvert );

    if ( source->hasDiffuse() )
      result->m_kd = source->m_diffuse;
    if ( source->hasSpecular() )
      result->m_ks = source->m_specular;
    if ( source->hasShininess() )
      result->m_ns = source->m_shininess;
    if ( source->hasOpacity() )
      result->m_alpha = source->m_opacity;

#ifdef RADIUM_WITH_TEXTURES
    if ( source->hasDiffuseTexture() )
      result->addTexture( Ra::Engine::BlinnPhongMaterial::TextureSemantic::TEX_DIFFUSE,
                          source->m_texDiffuse );
    if ( source->hasSpecularTexture() )
      result->addTexture( Ra::Engine::BlinnPhongMaterial::TextureSemantic::TEX_SPECULAR,
                          source->m_texSpecular );
    if ( source->hasShininessTexture() )
      result->addTexture( Ra::Engine::BlinnPhongMaterial::TextureSemantic::TEX_SHININESS,
                          source->m_texShininess );
    if ( source->hasOpacityTexture() )
      result->addTexture( Ra::Engine::BlinnPhongMaterial::TextureSemantic::TEX_ALPHA,
                          source->m_texOpacity );
    if ( source->hasNormalTexture() )
      result->addTexture( Ra::Engine::BlinnPhongMaterial::TextureSemantic::TEX_NORMAL,
                          source->m_texNormal );
#endif
    return result;
}

} // namespace Engine
} // namespace Ra
