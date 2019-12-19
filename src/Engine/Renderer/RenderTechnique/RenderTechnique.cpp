#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include "RenderTechnique.hpp"
#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Engine {

using namespace Core::Utils; // log

// For iterating on the enum easily
const std::array<RenderTechnique::PassName, 3> allPasses = {RenderTechnique::Z_PREPASS,
                                                            RenderTechnique::LIGHTING_OPAQUE,
                                                            RenderTechnique::LIGHTING_TRANSPARENT};

std::shared_ptr<Ra::Engine::RenderTechnique> RadiumDefaultRenderTechnique( nullptr );

RenderTechnique::RenderTechnique() {
    for ( auto p : allPasses )
    {
        m_shaders[p] = nullptr;
    }
}

RenderTechnique::RenderTechnique( const RenderTechnique& o ) :
    m_material{o.m_material}, m_dirtyBits{o.m_dirtyBits}, m_setPasses{o.m_setPasses} {
    for ( auto p : allPasses )
    {
        if ( m_setPasses & p )
        {
            m_shaderConfig[p] = o.m_shaderConfig.at( p );
            m_shaders[p]      = o.m_shaders.at( p );
        }
    }
}

RenderTechnique::~RenderTechnique() = default;

void RenderTechnique::setConfiguration( const ShaderConfiguration& newConfig, PassName pass ) {
    m_shaderConfig[pass] = newConfig;
    m_dirtyBits |= pass;
    m_setPasses |= pass;
}

const ShaderProgram* RenderTechnique::getShader( PassName pass ) const {
    if ( m_setPasses & pass ) { return m_shaders.at( pass ); }
    return nullptr;
}

void RenderTechnique::updateGL() {
    for ( auto p : allPasses )
    {
        if ( ( m_setPasses & p ) && ( ( nullptr == m_shaders[p] ) || ( m_dirtyBits & p ) ) )
        {
            m_shaders[p] =
                ShaderProgramManager::getInstance()->getShaderProgram( m_shaderConfig[p] );
            m_dirtyBits |= p;
        }
    }

    if ( m_material ) { m_material->updateGL(); }
}

const std::shared_ptr<Material>& RenderTechnique::getMaterial() const {
    return m_material;
}

void RenderTechnique::resetMaterial( Material* mat ) {
    m_material.reset( mat );
}

void RenderTechnique::setMaterial( const std::shared_ptr<Material>& material ) {
    RenderTechnique::m_material = material;
}

bool RenderTechnique::hasConfiguration( PassName pass ) const {
    return m_shaderConfig.find( pass ) != m_shaderConfig.end();
}

const ShaderConfiguration& RenderTechnique::getConfiguration( PassName pass ) const {
    return m_shaderConfig.at( pass );
}

// creates a Radium default rendertechnique :
//      Z_PREPASS = Nothing
//      LIGHTING_OPAQUE = BlinnPhong
//      LIGHTING_TRANSPARENT = Nothing
Ra::Engine::RenderTechnique RenderTechnique::createDefaultRenderTechnique() {
    if ( RadiumDefaultRenderTechnique != nullptr )
    { return *( RadiumDefaultRenderTechnique.get() ); }
    std::shared_ptr<Material> mat( new BlinnPhongMaterial( "DefaultGray" ) );
    auto rt = new Ra::Engine::RenderTechnique;
    rt->setMaterial( mat );
    auto builder = EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
    if ( !builder.first )
    {
        LOG( logERROR ) << "Unable to create the default technique : is the Engine initialized ? ";
    }
    builder.second( *rt, false );
    RadiumDefaultRenderTechnique.reset( rt );
    return *( RadiumDefaultRenderTechnique.get() );
}

bool RenderTechnique::shaderIsDirty( RenderTechnique::PassName pass ) const {
    return m_dirtyBits & pass;
}

///////////////////////////////////////////////
////        Radium defined technique        ///
///////////////////////////////////////////////
namespace EngineRenderTechniques {

/// Map that stores each technique builder function
static std::map<std::string, DefaultTechniqueBuilder> EngineTechniqueRegistry;

/** register a new default builder for a technique
 *  @return true if builder added, false else (e.g, a builder with the same name exists)
 */
bool registerDefaultTechnique( const std::string& name, DefaultTechniqueBuilder builder ) {
    auto result = EngineTechniqueRegistry.insert( {name, builder} );
    return result.second;
}

/** remove a default builder
 *  @return true if builder removed, false else (e.g, a builder with the same name does't exists)
 */
bool removeDefaultTechnique( const std::string& name ) {
    std::size_t removed = EngineTechniqueRegistry.erase( name );
    return ( removed == 1 );
}

/**
 * @param name name of the technique to construct
 * @return a pair containing the search result and, if true, the functor to call to build the
 * technique.
 */
std::pair<bool, DefaultTechniqueBuilder> getDefaultTechnique( const std::string& name ) {
    auto search = EngineTechniqueRegistry.find( name );
    if ( search != EngineTechniqueRegistry.end() ) { return {true, search->second}; }
    auto result = std::make_pair( false, [name]( RenderTechnique&, bool ) -> void {
        LOG( logERROR ) << "Undefined default technique for " << name << " !";
    } );
    return result;
}

bool cleanup() {
    EngineTechniqueRegistry.clear();
    return true;
}

} // namespace EngineRenderTechniques

} // namespace Engine
} // namespace Ra
