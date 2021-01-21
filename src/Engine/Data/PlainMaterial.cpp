#include <Engine/Data/PlainMaterial.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderTechnique.hpp>
#include <Engine/Renderer/ShaderConfigFactory.hpp>
#include <Engine/Renderer/ShaderProgram.hpp>
#include <Engine/Renderer/ShaderProgramManager.hpp>
#include <Engine/Scene/TextureManager.hpp>

namespace Ra {
namespace Engine {
namespace Data {
static const std::string materialName {"Plain"};

PlainMaterial::PlainMaterial( const std::string& instanceName ) :
    SimpleMaterial( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

PlainMaterial::~PlainMaterial() = default;

void PlainMaterial::registerMaterial() {
    // Get the Radium Resource location on the filesystem
    auto resourcesRootDir {RadiumEngine::getInstance()->getResourcesDir()};
    auto shaderProgramManager = RadiumEngine::getInstance()->getShaderProgramManager();

    shaderProgramManager->addNamedString( "/Plain.glsl",
                                          resourcesRootDir + "Shaders/Materials/Plain/Plain.glsl" );
    // registering re-usable shaders
    Renderer::ShaderConfiguration lpconfig(
        "Plain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.frag.glsl" );

    Renderer::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Renderer::ShaderConfiguration zprepassconfig(
        "ZprepassPlain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/PlainZPrepass.frag.glsl" );
    Renderer::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Renderer::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Renderer::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass = Renderer::ShaderConfigurationFactory::getConfiguration( "Plain" );
            rt.setConfiguration( *lightpass, Renderer::DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass =
                Renderer::ShaderConfigurationFactory::getConfiguration( "ZprepassPlain" );
            rt.setConfiguration( *zprepass, Renderer::DefaultRenderingPasses::Z_PREPASS );
        } );
}

void PlainMaterial::unregisterMaterial() {
    Renderer::EngineRenderTechniques::removeDefaultTechnique( "Plain" );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
