#include <Engine/Data/PlainMaterial.hpp>

#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgram.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>
#include <Engine/Scene/ShaderProgramManager.hpp>
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
    Rendering::ShaderConfiguration lpconfig(
        "Plain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.frag.glsl" );

    Rendering::ShaderConfigurationFactory::addConfiguration( lpconfig );

    Rendering::ShaderConfiguration zprepassconfig(
        "ZprepassPlain",
        resourcesRootDir + "Shaders/Materials/Plain/Plain.vert.glsl",
        resourcesRootDir + "Shaders/Materials/Plain/PlainZPrepass.frag.glsl" );
    Rendering::ShaderConfigurationFactory::addConfiguration( zprepassconfig );

    // Registering technique
    Rendering::EngineRenderTechniques::registerDefaultTechnique(
        materialName, []( Rendering::RenderTechnique& rt, bool ) {
            // Lighting pass
            auto lightpass = Rendering::ShaderConfigurationFactory::getConfiguration( "Plain" );
            rt.setConfiguration( *lightpass, Rendering::DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass
            auto zprepass =
                Rendering::ShaderConfigurationFactory::getConfiguration( "ZprepassPlain" );
            rt.setConfiguration( *zprepass, Rendering::DefaultRenderingPasses::Z_PREPASS );
        } );
}

void PlainMaterial::unregisterMaterial() {
    Rendering::EngineRenderTechniques::removeDefaultTechnique( "Plain" );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
