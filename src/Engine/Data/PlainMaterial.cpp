#include <Core/Material/SimpleMaterialModel.hpp>
#include <Engine/Data/MaterialConverters.hpp>
#include <Engine/Data/PlainMaterial.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>

#include <fstream>

namespace Ra {
namespace Engine {
namespace Data {

static const std::string materialName { "Plain" };

nlohmann::json PlainMaterial::s_parametersMetadata = {};

PlainMaterial::PlainMaterial( const std::string& instanceName ) :
    SimpleMaterial( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

PlainMaterial::~PlainMaterial() = default;

void PlainMaterial::registerMaterial() {
    // Get the Radium Resource location on the filesystem
    auto resourcesRootDir { RadiumEngine::getInstance()->getResourcesDir() };
    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();

    EngineMaterialConverters::registerMaterialConverter( materialName, PlainMaterialConverter() );

    shaderProgramManager->addNamedString( "/Plain.glsl",
                                          resourcesRootDir + "Shaders/Materials/Plain/Plain.glsl" );
    // registering re-usable shaders
    Data::ShaderConfiguration lpconfig(
        "Plain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.frag.glsl" );

    Data::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Data::ShaderConfiguration zprepassconfig(
        "ZprepassPlain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/PlainZPrepass.frag.glsl" );
    Data::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Rendering::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Rendering::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass = Data::ShaderConfigurationFactory::getConfiguration( "Plain" );
            rt.setConfiguration( *lightpass, Rendering::DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass = Data::ShaderConfigurationFactory::getConfiguration( "ZprepassPlain" );
            rt.setConfiguration( *zprepass, Rendering::DefaultRenderingPasses::Z_PREPASS );
        } );

    // Registering parameters metadata
    SimpleMaterial::loadMetaData( s_parametersMetadata );
}

void PlainMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( materialName );
    Rendering::EngineRenderTechniques::removeDefaultTechnique( "Plain" );
}

void PlainMaterial::updateFromParameters() {
    auto& renderParameters = getParameters();
    m_color                = renderParameters.getParameter<Core::Utils::Color>( "material.color" );
    m_perVertexColor       = renderParameters.getParameter<bool>( "material.perVertexColor" );
}

nlohmann::json PlainMaterial::getParametersMetadata() const {
    return s_parametersMetadata;
}

Material* PlainMaterialConverter::operator()( const Ra::Core::Material::MaterialModel* toconvert ) {
    auto result = new PlainMaterial( toconvert->getName() );
    // we are sure here that the concrete type of "toconvert" is BlinnPhongMaterialData
    // static cst is safe here
    auto source = static_cast<const Ra::Core::Material::SimpleMaterialModel*>( toconvert );

    result->m_color = source->m_kd;
    result->m_alpha = source->m_alpha;

    if ( source->hasDiffuseTexture() )
        result->addTexture( SimpleMaterial::TextureSemantic::TEX_COLOR, source->m_texDiffuse );
    if ( source->hasOpacityTexture() )
        result->addTexture( SimpleMaterial::TextureSemantic::TEX_MASK, source->m_texOpacity );

    return result;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
