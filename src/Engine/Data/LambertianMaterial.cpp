#include <Engine/Data/LambertianMaterial.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderTechnique.hpp>
#include <Engine/Renderer/ShaderConfigFactory.hpp>
#include <Engine/Renderer/ShaderProgram.hpp>
#include <Engine/Renderer/ShaderProgramManager.hpp>
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
    Renderer::ShaderConfiguration lpconfig(
        "Lambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.frag.glsl" );

    Renderer::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Renderer::ShaderConfiguration zprepassconfig(
        "ZprepassLambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/LambertianZPrepass.frag.glsl" );
    Renderer::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Renderer::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Renderer::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass = Renderer::ShaderConfigurationFactory::getConfiguration( "Lambertian" );
            rt.setConfiguration( *lightpass, Renderer::DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass =
                Renderer::ShaderConfigurationFactory::getConfiguration( "ZprepassLambertian" );
            rt.setConfiguration( *zprepass, Renderer::DefaultRenderingPasses::Z_PREPASS );
        } );
}

void LambertianMaterial::unregisterMaterial() {
    Renderer::EngineRenderTechniques::removeDefaultTechnique( materialName );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
