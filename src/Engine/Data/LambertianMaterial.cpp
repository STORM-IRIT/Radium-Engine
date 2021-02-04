#include <Engine/Data/LambertianMaterial.hpp>

#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgram.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>
#include <Engine/Scene/ShaderProgramManager.hpp>
#include <Engine/Scene/TextureManager.hpp>

namespace Ra {
namespace Engine {
namespace Data {
static const std::string materialName {"Lambertian"};

LambertianMaterial::LambertianMaterial( const std::string& instanceName ) :
    SimpleMaterial( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

LambertianMaterial::~LambertianMaterial() {}

void LambertianMaterial::registerMaterial() {
    // Get the Radium Resource location on the filesystem
    auto resourcesRootDir {RadiumEngine::getInstance()->getResourcesDir()};
    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();

    shaderProgramManager->addNamedString(
        "/Lambertian.glsl", resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.glsl" );
    // registering re-usable shaders
    Rendering::ShaderConfiguration lpconfig(
        "Lambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.frag.glsl" );

    Rendering::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Rendering::ShaderConfiguration zprepassconfig(
        "ZprepassLambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/LambertianZPrepass.frag.glsl" );
    Rendering::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Rendering::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Rendering::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass =
                Rendering::ShaderConfigurationFactory::getConfiguration( "Lambertian" );
            rt.setConfiguration( *lightpass, Rendering::DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass =
                Rendering::ShaderConfigurationFactory::getConfiguration( "ZprepassLambertian" );
            rt.setConfiguration( *zprepass, Rendering::DefaultRenderingPasses::Z_PREPASS );
        } );
}

void LambertianMaterial::unregisterMaterial() {
    Rendering::EngineRenderTechniques::removeDefaultTechnique( materialName );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
