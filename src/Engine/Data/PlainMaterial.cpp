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
    std::ifstream metadata( resourcesRootDir + "Metadata/Simple.json" );
    metadata >> s_parametersMetadata;
}

void PlainMaterial::unregisterMaterial() {
    Rendering::EngineRenderTechniques::removeDefaultTechnique( "Plain" );
}

void PlainMaterial::updateFromParameters() {
    m_color = m_renderParameters.getParameter<RenderParameters::ColorParameter>( "material.color" )
                  .m_value;
    m_perVertexColor =
        m_renderParameters
            .getParameter<RenderParameters::BoolParameter>( "material.perVertexColor" )
            .m_value;
}

nlohmann::json PlainMaterial::getParametersMetadata() const {
    return s_parametersMetadata;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
