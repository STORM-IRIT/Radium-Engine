#include <Engine/Renderer/Material/PlainMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <Core/Resources/Resources.hpp>

namespace Ra {
namespace Engine {

static const std::string materialName{"Plain"};

PlainMaterial::PlainMaterial( const std::string& instanceName ) :
    Material( instanceName, materialName, Material::MaterialAspect::MAT_OPAQUE ) {}

PlainMaterial::~PlainMaterial() {
    m_textures.clear();
}

void PlainMaterial::updateGL() {
    if ( !m_isDirty ) { return; }

    // Load textures
    TextureManager* texManager = TextureManager::getInstance();
    for ( const auto& tex : m_pendingTextures )
    {
        // ask to convert color textures from sRGB to Linear RGB
        bool tolinear         = ( tex.first == TextureSemantic::TEX_COLOR );
        auto texture          = texManager->getOrLoadTexture( tex.second, tolinear );
        m_textures[tex.first] = texture;
        // do not call addTexture since it invalidate m_pendingTextures itr
        //       addTexture( tex.first, texture );
    }

    m_pendingTextures.clear();
    m_isDirty = false;
}

void PlainMaterial::bind( const ShaderProgram* shader ) {
    shader->setUniform( "material.color", m_color );
    shader->setUniform( "material.perVertexColor", m_perVertexColor );
    shader->setUniform( "material.shaded", m_shaded );

    Texture* tex = getTexture( PlainMaterial::TextureSemantic::TEX_COLOR );
    if ( tex != nullptr )
    {
        shader->setUniformTexture( "material.tex.color", tex );
        shader->setUniform( "material.tex.hasColor", 1 );
    }
    else
    { shader->setUniform( "material.tex.hasColor", 0 ); }

    tex = getTexture( PlainMaterial::TextureSemantic::TEX_MASK );
    if ( tex != nullptr )
    {
        shader->setUniformTexture( "material.tex.mask", tex );
        shader->setUniform( "material.tex.hasMask", 1 );
    }
    else
    { shader->setUniform( "material.tex.hasMask", 0 ); }
}

void PlainMaterial::registerMaterial() {
    /// For internal resources management in a filesystem
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
        materialName,

        []( Ra::Engine::RenderTechnique& rt, bool isTransparent ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory) : Plain
            auto lightpassconfig =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" );
            rt.setConfiguration( lightpassconfig, Ra::Engine::RenderTechnique::LIGHTING_OPAQUE );

            // Z prepass (Recommended)
            auto zprepassconfig =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "ZprepassPlain" );
            rt.setConfiguration( zprepassconfig, Ra::Engine::RenderTechnique::Z_PREPASS );
        } );
}

void PlainMaterial::unregisterMaterial() {
    EngineRenderTechniques::removeDefaultTechnique( materialName );
}

} // namespace Engine
} // namespace Ra
