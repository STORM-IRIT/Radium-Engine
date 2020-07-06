#include <Engine/Renderer/Material/PlainMaterial.hpp>

#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <Core/Resources/Resources.hpp>

namespace Ra {
namespace Engine {

static const std::string materialName {"Plain"};

PlainMaterial::PlainMaterial( const std::string& instanceName ) :
    SimpleMaterial( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

PlainMaterial::~PlainMaterial() = default;

void PlainMaterial::registerMaterial() {
    // Get the Radium Resource location on the filesystem
    std::string resourcesRootDir = {Core::Resources::getRadiumResourcesDir()};

    ShaderProgramManager::getInstance()->addNamedString(
        "/Plain.glsl", resourcesRootDir + "Shaders/Materials/Plain/Plain.glsl" );
    // registering re-usable shaders
    Ra::Engine::ShaderConfiguration lpconfig(
        "Plain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.frag.glsl" );

    Ra::Engine::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Ra::Engine::ShaderConfiguration zprepassconfig(
        "ZprepassPlain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/PlainZPrepass.frag.glsl" );
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Ra::Engine::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Ra::Engine::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass = Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" );
            rt.setConfiguration( *lightpass, DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "ZprepassPlain" );
            rt.setConfiguration( *zprepass, DefaultRenderingPasses::Z_PREPASS );
        } );
}

void PlainMaterial::unregisterMaterial() {
    EngineRenderTechniques::removeDefaultTechnique( "Plain" );
}

} // namespace Engine
} // namespace Ra
