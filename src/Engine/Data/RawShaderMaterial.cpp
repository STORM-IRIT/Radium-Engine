#include <Engine/Data/RawShaderMaterial.hpp>

#include <Engine/Data/MaterialConverters.hpp>
#include <Engine/Renderer/RenderTechnique.hpp>
#include <Engine/Renderer/ShaderConfigFactory.hpp>

namespace Ra {
namespace Engine {
namespace Data {
RawShaderMaterial::RawShaderMaterial(
    const std::string& instanceName,
    const std::vector<std::pair<Renderer::ShaderType, std::string>>& shaders,
    std::shared_ptr<Renderer::ShaderParameterProvider> paramProvider ) :
    Material( instanceName, instanceName, Material::MaterialAspect::MAT_OPAQUE ),
    m_shaders {shaders},
    m_paramProvider {std::move( paramProvider )} {
    m_materialKey = computeKey();
    setMaterialName( m_materialKey );
    registerDefaultTechnique();
}

RawShaderMaterial::~RawShaderMaterial() {
    Renderer::EngineRenderTechniques::removeDefaultTechnique( m_materialKey );
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
    Renderer::ShaderConfiguration myConfig {m_materialKey};
    for ( const auto& p : m_shaders )
    {
        myConfig.addShaderSource( p.first, p.second );
    }
    Renderer::ShaderConfigurationFactory::addConfiguration( myConfig );
    // Register the technique builder for the custom material
    // For now, as we can't change the material name, always use the key of the initial
    // configuration
    auto materialKey {m_materialKey};

    Renderer::EngineRenderTechniques::registerDefaultTechnique(
        materialKey, [materialKey]( Renderer::RenderTechnique& rt, bool ) {
            // Configure the technique to render this object using forward Renderer or any
            // compatible one Main pass (Mandatory) : BlinnPhong
            auto pass = Renderer::ShaderConfigurationFactory::getConfiguration( materialKey );
            rt.setConfiguration( *pass, Renderer::DefaultRenderingPasses::LIGHTING_OPAQUE );
            // Z prepass use the same config
            rt.setConfiguration( *pass, Renderer::DefaultRenderingPasses::Z_PREPASS );
        } );
}

void RawShaderMaterial::registerMaterial() {
    // Defining the material converter
    EngineMaterialConverters::registerMaterialConverter( "Renderer::RawShaderMaterialData",
                                                         RawShaderMaterialConverter() );
}

void RawShaderMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( "Renderer::RawShaderMaterialData" );
}

void RawShaderMaterial::updateGL() {
    // update the state of the parameter provider
    m_paramProvider->updateGL();
}

void RawShaderMaterial::updateShaders(
    const std::vector<std::pair<Renderer::ShaderType, std::string>>& shaders,
    std::shared_ptr<Renderer::ShaderParameterProvider> paramProvider ) {
    Renderer::ShaderConfigurationFactory::removeConfiguration( m_materialKey );
    Renderer::EngineRenderTechniques::removeDefaultTechnique( m_materialKey );
    m_shaders = shaders;
    if ( paramProvider ) { m_paramProvider = std::move( paramProvider ); }
    m_materialKey = computeKey();
    setMaterialName( m_materialKey );
    registerDefaultTechnique();
}
} // namespace Data
} // namespace Engine
} // namespace Ra
