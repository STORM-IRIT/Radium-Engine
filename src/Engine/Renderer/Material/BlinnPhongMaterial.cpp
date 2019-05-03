#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/MaterialConverters.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <Core/Asset/BlinnPhongMaterialData.hpp>

namespace Ra {
namespace Engine {

BlinnPhongMaterial::BlinnPhongMaterial( const std::string& name ) :
    Material( name, Material::MaterialAspect::MAT_OPAQUE ) {}

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
        // ask to convert color textures from sRGB to Linear RGB
        bool tolinear = ( tex.first == TextureSemantic::TEX_DIFFUSE ||
                          tex.first == TextureSemantic::TEX_SPECULAR );
        auto texture = texManager->getOrLoadTexture( tex.second, tolinear );
        m_textures[tex.first] = texture;
        // do not call addTexture since it invalidate m_pendingTextures itr
        //       addTexture( tex.first, texture );
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
    shader->setUniform( "material.alpha", std::min( m_alpha, m_kd[3] ) );

    Texture* tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_DIFFUSE );
    if ( tex != nullptr )
    {
        shader->setUniformTexture( "material.tex.kd", tex );
        shader->setUniform( "material.tex.hasKd", 1 );
    } else
    { shader->setUniform( "material.tex.hasKd", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_SPECULAR );
    if ( tex != nullptr )
    {
        shader->setUniformTexture( "material.tex.ks", tex );
        shader->setUniform( "material.tex.hasKs", 1 );
    } else
    { shader->setUniform( "material.tex.hasKs", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_NORMAL );
    if ( tex != nullptr )
    {
        shader->setUniformTexture( "material.tex.normal", tex );
        shader->setUniform( "material.tex.hasNormal", 1 );
    } else
    { shader->setUniform( "material.tex.hasNormal", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_SHININESS );
    if ( tex != nullptr )
    {
        shader->setUniformTexture( "material.tex.ns", tex );
        shader->setUniform( "material.tex.hasNs", 1 );
    } else
    { shader->setUniform( "material.tex.hasNs", 0 ); }

    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_ALPHA );
    if ( tex != nullptr )
    {
        shader->setUniformTexture( "material.tex.alpha", tex );
        shader->setUniform( "material.tex.hasAlpha", 1 );
    } else
    { shader->setUniform( "material.tex.hasAlpha", 0 ); }
}

bool BlinnPhongMaterial::isTransparent() const {
    return ( m_alpha < 1.0 ) || ( m_kd[3] < 1.0 ) || Material::isTransparent();
}

void BlinnPhongMaterial::registerMaterial() {
    // Defining Converter
    EngineMaterialConverters::registerMaterialConverter( "BlinnPhong",
                                                         BlinnPhongMaterialConverter() );

    ShaderProgramManager::getInstance()->addNamedString("/BlinnPhongMaterial.glsl",
        "Shaders/Materials/BlinnPhong/BlinnPhongMaterial.glsl");
    // registering re-usable shaders
    Ra::Engine::ShaderConfiguration lpconfig( "BlinnPhong",
                                              "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                                              "Shaders/Materials/BlinnPhong/BlinnPhong.frag.glsl" );

    Ra::Engine::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Ra::Engine::ShaderConfiguration zprepassconfig(
        "ZprepassBlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
        "Shaders/Materials/BlinnPhong/DepthAmbientBlinnPhong.frag.glsl" );
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    Ra::Engine::ShaderConfiguration transparentpassconfig(
        "LitOITBlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
        "Shaders/Materials/BlinnPhong/LitOITBlinnPhong.frag.glsl" );
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( transparentpassconfig );

    // Registering technique
    Ra::Engine::EngineRenderTechniques::registerDefaultTechnique(
        "BlinnPhong",

        []( Ra::Engine::RenderTechnique& rt, bool isTransparent ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory) : BlinnPhong
            auto lightpassconfig =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "BlinnPhong" );
            rt.setConfiguration( lightpassconfig, Ra::Engine::RenderTechnique::LIGHTING_OPAQUE );

            // Z prepass (Recommended) : DepthAmbiantPass
            auto zprepassconfig =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "ZprepassBlinnPhong" );
            rt.setConfiguration( zprepassconfig, Ra::Engine::RenderTechnique::Z_PREPASS );
            // Transparent pass (0ptional) : If Transparent ... add LitOIT
            if ( isTransparent )
            {
                auto transparentpassconfig =
                    Ra::Engine::ShaderConfigurationFactory::getConfiguration( "LitOITBlinnPhong" );
                rt.setConfiguration( transparentpassconfig,
                                     Ra::Engine::RenderTechnique::LIGHTING_TRANSPARENT );
            }
        } );
}

void BlinnPhongMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( "BlinnPhong" );
    EngineRenderTechniques::removeDefaultTechnique( "BlinnPhong" );
}

Material* BlinnPhongMaterialConverter::
operator()( const Ra::Core::Asset::MaterialData* toconvert ) {
    auto result = new Ra::Engine::BlinnPhongMaterial( toconvert->getName() );
    // we are sure here that the concrete type of "toconvert" is BlinnPhongMaterialData
    // static cst is safe here
    auto source = static_cast<const Ra::Core::Asset::BlinnPhongMaterialData*>( toconvert );

    if ( source->hasDiffuse() )
        result->m_kd = source->m_diffuse;
    if ( source->hasSpecular() )
        result->m_ks = source->m_specular;
    if ( source->hasShininess() )
        result->m_ns = source->m_shininess;
    if ( source->hasOpacity() )
        result->m_alpha = source->m_opacity;
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

    return result;
}

} // namespace Engine
} // namespace Ra
