#include <Engine/Data/LambertianMaterial.hpp>

#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>

#include <fstream>

namespace Ra {
namespace Engine {
namespace Data {
static const std::string materialName { "Lambertian" };

nlohmann::json LambertianMaterial::s_parametersMetadata = {};

LambertianMaterial::LambertianMaterial( const std::string& instanceName ) :
    SimpleMaterial( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

LambertianMaterial::~LambertianMaterial() {}

void LambertianMaterial::registerMaterial() {
    // Get the Radium Resource location on the filesystem
    auto resourcesRootDir { RadiumEngine::getInstance()->getResourcesDir() };
    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();

    shaderProgramManager->addNamedString(
        "/Lambertian.glsl", resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.glsl" );
    // registering re-usable shaders
    Data::ShaderConfiguration lpconfig(
        "Lambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.frag.glsl" );

    Data::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Data::ShaderConfiguration zprepassconfig(
        "ZprepassLambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/LambertianZPrepass.frag.glsl" );
    Data::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Rendering::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Rendering::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass = Data::ShaderConfigurationFactory::getConfiguration( "Lambertian" );
            rt.setConfiguration( *lightpass, Rendering::DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass =
                Data::ShaderConfigurationFactory::getConfiguration( "ZprepassLambertian" );
            rt.setConfiguration( *zprepass, Rendering::DefaultRenderingPasses::Z_PREPASS );
        } );

    // Registering parameters metadata
    std::ifstream metadata( resourcesRootDir + "Metadata/Simple.json" );
    metadata >> s_parametersMetadata;
}

void LambertianMaterial::unregisterMaterial() {
    Rendering::EngineRenderTechniques::removeDefaultTechnique( materialName );
}

void LambertianMaterial::updateFromParameters() {
    m_color = m_renderParameters.getParameter<RenderParameters::ColorParameter>( "material.color" )
                  .m_value;
    m_perVertexColor =
        m_renderParameters
            .getParameter<RenderParameters::BoolParameter>( "material.perVertexColor" )
            .m_value;
}

nlohmann::json LambertianMaterial::getParametersMetadata() const {
    return s_parametersMetadata;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
