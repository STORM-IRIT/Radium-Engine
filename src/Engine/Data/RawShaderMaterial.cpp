#include <Engine/Data/RawShaderMaterial.hpp>

#include <Engine/Data/MaterialConverters.hpp>
#include <Engine/Renderer/RenderTechnique.hpp>
#include <Engine/Renderer/ShaderConfigFactory.hpp>

namespace Ra {
namespace Engine {

RawShaderMaterial::RawShaderMaterial(
    const std::string& instanceName,
    const std::vector<std::pair<Ra::Engine::ShaderType, std::string>>& shaders,
    std::shared_ptr<Ra::Engine::ShaderParameterProvider> paramProvider ) :
    Material( instanceName, instanceName, Material::MaterialAspect::MAT_OPAQUE ),
    m_shaders {shaders},
    m_paramProvider {std::move( paramProvider )} {
    m_materialKey = computeKey();
    setMaterialName( m_materialKey );
    registerDefaultTechnique();
}

RawShaderMaterial::~RawShaderMaterial() {
    EngineRenderTechniques::removeDefaultTechnique( m_materialKey );
}

std::string RawShaderMaterial::computeKey() {
    // Hash the shader source to obtain identification keys
    auto content = std::to_string( std::size_t( this ) );
    for ( const auto& p : m_shaders )
    {
        content += p.second;
        content += std::to_string( std::size_t( this ) );
    }
    return std::to_string( std::hash<std::string> {}( content ) );
}

void RawShaderMaterial::registerDefaultTechnique() {
    // Generate configuration using the given glsl source.
    // The configuration key/name is the hash of shader sources
    // The same configuration will be used as z-prepass config and opaque pass config.
    Ra::Engine::ShaderConfiguration myConfig {m_materialKey};
    for ( const auto& p : m_shaders )
    {
        myConfig.addShaderSource( p.first, p.second );
    }
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( myConfig );
    // Register the technique builder for the custom material
    // For now, as we can't change the material name, always use the key of the initial
    // configuration
    auto materialKey {m_materialKey};

    Ra::Engine::EngineRenderTechniques::registerDefaultTechnique(
        materialKey, [materialKey]( Ra::Engine::RenderTechnique& rt, bool ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory) : BlinnPhong
            auto pass = Ra::Engine::ShaderConfigurationFactory::getConfiguration( materialKey );
            rt.setConfiguration( *pass, DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass use the same config
            rt.setConfiguration( *pass, DefaultRenderingPasses::Z_PREPASS );
        } );
}

void RawShaderMaterial::registerMaterial() {
    // Defining the material converter
    EngineMaterialConverters::registerMaterialConverter( "Ra::Engine::RawShaderMaterialData",
                                                         RawShaderMaterialConverter() );
}

void RawShaderMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( "Ra::Engine::RawShaderMaterialData" );
}

void RawShaderMaterial::updateGL() {
    // update the state of the parameter provider
    m_paramProvider->updateGL();
}

void RawShaderMaterial::updateShaders(
    const std::vector<std::pair<Ra::Engine::ShaderType, std::string>>& shaders,
    std::shared_ptr<Ra::Engine::ShaderParameterProvider> paramProvider ) {
    Ra::Engine::ShaderConfigurationFactory::removeConfiguration( m_materialKey );
    Ra::Engine::EngineRenderTechniques::removeDefaultTechnique( m_materialKey );
    m_shaders = shaders;
    if ( paramProvider ) { m_paramProvider = std::move( paramProvider ); }
    m_materialKey = computeKey();
    setMaterialName( m_materialKey );
    registerDefaultTechnique();
}

} // namespace Engine
} // namespace Ra
