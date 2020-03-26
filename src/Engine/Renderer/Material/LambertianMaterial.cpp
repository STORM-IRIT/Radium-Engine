#include <Engine/Renderer/Material/LambertianMaterial.hpp>

#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <Core/Resources/Resources.hpp>

namespace Ra {
namespace Engine {

static const std::string materialName{"Lambertian"};

LambertianMaterial::LambertianMaterial( const std::string& instanceName ) :
    SimpleMaterial( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

LambertianMaterial::~LambertianMaterial() {}

void LambertianMaterial::registerMaterial() {
    // Get the Radium Resource location on the filesystem
    std::string resourcesRootDir = {Core::Resources::getRadiumResourcesDir()};

    ShaderProgramManager::getInstance()->addNamedString(
        "/Lambertian.glsl", resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.glsl" );
    // registering re-usable shaders
    Ra::Engine::ShaderConfiguration lpconfig(
        "Lambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.frag.glsl" );

    Ra::Engine::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Ra::Engine::ShaderConfiguration zprepassconfig(
        "ZprepassLambertian",
        resourcesRootDir + "Shaders/Materials/Lambertian/Lambertian.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Lambertian/LambertianZPrepass.frag.glsl" );
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Ra::Engine::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Ra::Engine::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Lambertian" );
            rt.setConfiguration( *lightpass, DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "ZprepassLambertian" );
            rt.setConfiguration( *zprepass, DefaultRenderingPasses::Z_PREPASS );
        } );
}

void LambertianMaterial::unregisterMaterial() {
    EngineRenderTechniques::removeDefaultTechnique( materialName );
}

} // namespace Engine
} // namespace Ra
