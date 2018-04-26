#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Engine {

// For iterating on the enum easily
const std::array<RenderTechnique::PassName, 3> allPasses = {RenderTechnique::Z_PREPASS,
                                                            RenderTechnique::LIGHTING_OPAQUE,
                                                            RenderTechnique::LIGHTING_TRANSPARENT};

std::shared_ptr<Ra::Engine::RenderTechnique> RadiumDefaultRenderTechnique( nullptr );

RenderTechnique::RenderTechnique() {
    for ( auto p : allPasses )
    {
        shaders[p] = nullptr;
    }
}

RenderTechnique::RenderTechnique( const RenderTechnique& o ) {
    material = o.material;
    dirtyBits = o.dirtyBits;
    setPasses = o.setPasses;

    for ( auto p : allPasses )
    {
        if ( setPasses & p )
        {
            shaderConfig[p] = o.shaderConfig.at( p );
            shaders[p] = o.shaders.at( p );
        }
    }
}

RenderTechnique::~RenderTechnique() {}

void RenderTechnique::setConfiguration( const ShaderConfiguration& newConfig, PassName pass ) {
    shaderConfig[pass] = newConfig;
    dirtyBits |= pass;
    setPasses |= pass;
}

const ShaderProgram* RenderTechnique::getShader( PassName pass ) const {
    if ( setPasses & pass )
    {
        return shaders.at( pass );
    }
    return nullptr;
}

void RenderTechnique::updateGL() {
    for ( auto p : allPasses )
    {
        if ( ( setPasses & p ) && ( ( nullptr == shaders[p] ) || ( dirtyBits & p ) ) )
        {
            shaders[p] = ShaderProgramManager::getInstance()->getShaderProgram( shaderConfig[p] );
            dirtyBits |= p;
        }
    }

    if ( material )
    {
        material->updateGL();
    }
}

const std::shared_ptr<Material>& RenderTechnique::getMaterial() const {
    return material;
}

void RenderTechnique::resetMaterial( Material* mat ) {
    material.reset( mat );
}

void RenderTechnique::setMaterial( const std::shared_ptr<Material>& material ) {
    RenderTechnique::material = material;
}

ShaderConfiguration RenderTechnique::getConfiguration( PassName pass ) const {
    return shaderConfig.at( pass );
}

// creates a Radium default rendertechnique :
//      Z_PREPASS = Nothing
//      LIGHTING_OPAQUE = BlinnPhong
//      LIGHTING_TRANSPARENT = Nothing
Ra::Engine::RenderTechnique RenderTechnique::createDefaultRenderTechnique() {
    if ( RadiumDefaultRenderTechnique != nullptr )
    {
        return *( RadiumDefaultRenderTechnique.get() );
    }

    Ra::Engine::RenderTechnique* rt = new Ra::Engine::RenderTechnique;
    auto config = ShaderConfigurationFactory::getConfiguration( "BlinnPhong" );
    rt->setConfiguration( config, LIGHTING_OPAQUE );
    std::shared_ptr<Material> mat( new BlinnPhongMaterial( "DefaultGray" ) );
    rt->setMaterial( mat );
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
    if ( search != EngineTechniqueRegistry.end() )
    {
        return {true, search->second};
    }
    auto result = std::make_pair( false, [name]( RenderTechnique&, bool ) -> void {
        LOG( logERROR ) << "Undefined default technique for " << name << " !";
    } );
    return result;
}

} // namespace EngineRenderTechniques

} // namespace Engine
} // namespace Ra
