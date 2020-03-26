#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Engine {

using namespace Core::Utils; // log

std::shared_ptr<Ra::Engine::RenderTechnique> RadiumDefaultRenderTechnique{nullptr};

RenderTechnique::RenderTechnique() : m_numActivePass{0} {
    for ( auto p = Index( 0 ); p < m_maxNbPasses; ++p )
    {
        m_activePasses[p]     = std::move( PassConfiguration( ShaderConfiguration(), nullptr ) );
        m_passesParameters[p] = nullptr;
    }
}

RenderTechnique::RenderTechnique( const RenderTechnique& o ) :
    m_numActivePass{o.m_numActivePass}, m_dirtyBits{o.m_dirtyBits}, m_setPasses{o.m_setPasses} {
    for ( auto p = Index( 0 ); p < m_numActivePass; ++p )
    {
        if ( m_setPasses & ( 1 << p ) )
        {
            m_activePasses[p]     = o.m_activePasses[p];
            m_passesParameters[p] = o.m_passesParameters[p];
        }
    }
}

RenderTechnique::~RenderTechnique() = default;

void RenderTechnique::setConfiguration( const ShaderConfiguration& newConfig,
                                        Core::Utils::Index pass ) {
    m_numActivePass      = std::max( m_numActivePass, pass + 1 );
    m_activePasses[pass] = std::move( PassConfiguration( newConfig, nullptr ) );
    m_dirtyBits |= ( 1 << pass );
    m_setPasses |= ( 1 << pass );
}

const ShaderProgram* RenderTechnique::getShader( Core::Utils::Index pass ) const {
    if ( m_setPasses & ( 1 << pass ) ) { return m_activePasses[pass].second; }
    return nullptr;
}

void RenderTechnique::setParametersProvider(
    const std::shared_ptr<ShaderParameterProvider>& provider,
    Core::Utils::Index pass ) {
    if ( m_numActivePass == 0 )
    {
        LOG( logERROR )
            << "Unable to set pass parameters : is passes configured using setConfiguration ? ";
        return;
    }
    if ( pass.isValid() ) { m_passesParameters[pass] = provider; }
    else
    {
        for ( int i = 0; i < m_numActivePass; ++i )
        {
            m_passesParameters[i] = provider;
        }
    }
}

const ShaderParameterProvider*
RenderTechnique::getParametersProvider( Core::Utils::Index pass ) const {
    if ( m_setPasses & ( 1 << pass ) ) { return m_passesParameters[pass].get(); }
    return nullptr;
}

void RenderTechnique::updateGL() {
    for ( auto p = Index( 0 ); p < m_numActivePass; ++p )
    {
        if ( ( m_setPasses & ( 1 << p ) ) &&
             ( ( nullptr == m_activePasses[p].second ) || ( m_dirtyBits & ( 1 << p ) ) ) )
        {
            m_activePasses[p].second =
                ShaderProgramManager::getInstance()->getShaderProgram( m_activePasses[p].first );
            m_dirtyBits |= ( 1 << p );
        }
    }
    for ( auto p = Index( 0 ); p < m_numActivePass; ++p )
    {
        if ( m_passesParameters[p] ) { m_passesParameters[p]->updateGL(); }
    }
}

bool RenderTechnique::hasConfiguration( Core::Utils::Index pass ) const {
    return m_setPasses & ( 1 << pass );
}

const ShaderConfiguration& RenderTechnique::getConfiguration( Core::Utils::Index pass ) const {
    return m_activePasses[pass].first;
}

bool RenderTechnique::shaderIsDirty( Core::Utils::Index pass ) const {
    return m_dirtyBits & ( 1 << pass );
}

///////////////////////////////////////////////
Ra::Engine::RenderTechnique RenderTechnique::createDefaultRenderTechnique() {
    if ( RadiumDefaultRenderTechnique != nullptr )
    { return *( RadiumDefaultRenderTechnique.get() ); }
    std::shared_ptr<Material> mat( new BlinnPhongMaterial( "DefaultGray" ) );
    auto rt      = new Ra::Engine::RenderTechnique;
    auto builder = EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
    if ( !builder.first )
    {
        LOG( logERROR ) << "Unable to create the default technique : is the Engine initialized ? ";
    }
    builder.second( *rt, false );
    rt->setParametersProvider( mat );
    RadiumDefaultRenderTechnique.reset( rt );
    return *( RadiumDefaultRenderTechnique.get() );
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
