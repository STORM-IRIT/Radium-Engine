#include <Core/Asset/BlinnPhongMaterialData.hpp>
#include <Engine/Data/BlinnPhongMaterial.hpp>
#include <Engine/Data/MaterialConverters.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>

#include <fstream>

namespace Ra {
namespace Engine {
namespace Data {
static const std::string materialName { "BlinnPhong" };

nlohmann::json BlinnPhongMaterial::s_parametersMetadata = {};

BlinnPhongMaterial::BlinnPhongMaterial( const std::string& instanceName ) :
    Material( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

void BlinnPhongMaterial::updateRenderingParameters() {
    // update the rendering parameters
    auto& renderParameters = getParameters();
    renderParameters.addParameter( "material.kd", m_kd );
    renderParameters.addParameter( "material.hasPerVertexKd", m_perVertexColor );
    renderParameters.addParameter( "material.renderAsSplat", m_renderAsSplat );
    renderParameters.addParameter( "material.ks", m_ks );
    renderParameters.addParameter( "material.ns", m_ns );
    renderParameters.addParameter( "material.alpha", std::min( m_alpha, m_kd[3] ) );
    Texture* tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_DIFFUSE );
    if ( tex != nullptr ) { renderParameters.addParameter( "material.tex.kd", tex ); }
    renderParameters.addParameter( "material.tex.hasKd", tex != nullptr );
    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_SPECULAR );
    if ( tex != nullptr ) { renderParameters.addParameter( "material.tex.ks", tex ); }
    renderParameters.addParameter( "material.tex.hasKs", tex != nullptr );
    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_NORMAL );
    if ( tex != nullptr ) { renderParameters.addParameter( "material.tex.normal", tex ); }
    renderParameters.addParameter( "material.tex.hasNormal", tex != nullptr );
    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_SHININESS );
    if ( tex != nullptr ) { renderParameters.addParameter( "material.tex.ns", tex ); }
    renderParameters.addParameter( "material.tex.hasNs", tex != nullptr );
    tex = getTexture( BlinnPhongMaterial::TextureSemantic::TEX_ALPHA );
    if ( tex != nullptr ) { renderParameters.addParameter( "material.tex.alpha", tex ); }
    renderParameters.addParameter( "material.tex.hasAlpha", tex != nullptr );
}

void BlinnPhongMaterial::updateGL() {
    if ( !isDirty() ) { return; }

    updateRenderingParameters();
    setClean();
}

void BlinnPhongMaterial::updateFromParameters() {
    auto& renderParameters = getParameters();
    m_kd                   = renderParameters.getParameter<Core::Utils::Color>( "material.kd" );
    m_perVertexColor       = renderParameters.getParameter<bool>( "material.hasPerVertexKd" );
    m_renderAsSplat        = renderParameters.getParameter<bool>( "material.renderAsSplat" );
    m_ks                   = renderParameters.getParameter<Core::Utils::Color>( "material.ks" );
    m_ns                   = renderParameters.getParameter<Scalar>( "material.ns" );
    m_alpha                = renderParameters.getParameter<Scalar>( "material.alpha" );
}

bool BlinnPhongMaterial::isTransparent() const {
    return ( m_alpha < 1_ra ) || ( m_kd[3] < 1_ra ) || Material::isTransparent();
}

void BlinnPhongMaterial::registerMaterial() {
    // For resources access (glsl files) in a filesystem
    auto resourcesRootDir { RadiumEngine::getInstance()->getResourcesDir() };
    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();

    // Defining the material converter
    EngineMaterialConverters::registerMaterialConverter( materialName,
                                                         BlinnPhongMaterialConverter() );

    // adding the material glsl implementation file
    shaderProgramManager->addNamedString(
        "/BlinnPhong.glsl", resourcesRootDir + "Shaders/Materials/BlinnPhong/BlinnPhong.glsl" );
    // registering re-usable shaders
    Data::ShaderConfiguration lpconfig(
        "BlinnPhong",
        resourcesRootDir + "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
        resourcesRootDir + "Shaders/Materials/BlinnPhong/BlinnPhong.frag.glsl" );
    Data::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Data::ShaderConfiguration zprepassconfig(
        "ZprepassBlinnPhong",
        resourcesRootDir + "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
        resourcesRootDir + "Shaders/Materials/BlinnPhong/BlinnPhongZPrepass.frag.glsl" );
    Data::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    Data::ShaderConfiguration transparentpassconfig(
        "LitOITBlinnPhong",
        resourcesRootDir + "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
        resourcesRootDir + "Shaders/Materials/BlinnPhong/LitOITBlinnPhong.frag.glsl" );
    Data::ShaderConfigurationFactory::addConfiguration( transparentpassconfig );

    // Registering technique
    Rendering::EngineRenderTechniques::registerDefaultTechnique(
        materialName,

        []( Rendering::RenderTechnique& rt, bool isTransparent ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory) : BlinnPhong
            auto lightpass = Data::ShaderConfigurationFactory::getConfiguration( "BlinnPhong" );
            rt.setConfiguration( *lightpass, Rendering::DefaultRenderingPasses::LIGHTING_OPAQUE );

            // Z prepass (Recommended) : DepthAmbiantPass
            auto zprepass =
                Data::ShaderConfigurationFactory::getConfiguration( "ZprepassBlinnPhong" );
            rt.setConfiguration( *zprepass, Rendering::DefaultRenderingPasses::Z_PREPASS );
            // Transparent pass (0ptional) : If Transparent ... add LitOIT
            if ( isTransparent ) {
                auto transparentpass =
                    Data::ShaderConfigurationFactory::getConfiguration( "LitOITBlinnPhong" );
                rt.setConfiguration( *transparentpass,
                                     Rendering::DefaultRenderingPasses::LIGHTING_TRANSPARENT );
            }
        } );

    // Registering parameters metadata
    ParameterSetEditingInterface::loadMetaData( materialName, s_parametersMetadata );
}

void BlinnPhongMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( materialName );
    Rendering::EngineRenderTechniques::removeDefaultTechnique( materialName );
}

Material*
BlinnPhongMaterialConverter::operator()( const Ra::Core::Asset::MaterialData* toconvert ) {
    auto result = new BlinnPhongMaterial( toconvert->getName() );
    // we are sure here that the concrete type of "toconvert" is BlinnPhongMaterialData
    // static cst is safe here
    auto source = static_cast<const Ra::Core::Asset::BlinnPhongMaterialData*>( toconvert );

    if ( source->hasDiffuse() ) result->m_kd = source->m_diffuse;
    if ( source->hasSpecular() ) result->m_ks = source->m_specular;
    if ( source->hasShininess() ) result->m_ns = source->m_shininess;
    if ( source->hasOpacity() ) result->m_alpha = source->m_opacity;
    if ( source->hasDiffuseTexture() )
        result->addTexture( BlinnPhongMaterial::TextureSemantic::TEX_DIFFUSE,
                            source->m_texDiffuse );
    if ( source->hasSpecularTexture() )
        result->addTexture( BlinnPhongMaterial::TextureSemantic::TEX_SPECULAR,
                            source->m_texSpecular );
    if ( source->hasShininessTexture() )
        result->addTexture( BlinnPhongMaterial::TextureSemantic::TEX_SHININESS,
                            source->m_texShininess );
    if ( source->hasOpacityTexture() )
        result->addTexture( BlinnPhongMaterial::TextureSemantic::TEX_ALPHA, source->m_texOpacity );
    if ( source->hasNormalTexture() )
        result->addTexture( BlinnPhongMaterial::TextureSemantic::TEX_NORMAL, source->m_texNormal );

    return result;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
