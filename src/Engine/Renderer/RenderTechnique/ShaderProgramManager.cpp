#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Log/Log.hpp>

#include <globjects/NamedString.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/base/File.h>

namespace Ra {
namespace Engine {
using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;

ShaderProgramManager::ShaderProgramManager( const std::string& vs, const std::string& fs ) :
    m_defaultVsName { vs },
    m_defaultFsName { fs } {
    initialize();
}

ShaderProgramManager::~ShaderProgramManager() {
    m_shaderPrograms.clear();
}

void ShaderProgramManager::initialize() {
    // Create named strings which correspond to shader files that you want to use in shaders's
    // includes. NOTE: if you want to add a named string to handle a new shader include file, be
    // SURE that the name (first parameter) begin with a "/", otherwise it won't work !
    // Radium V2 : are these initialization required here ? They will be better in Engine::Initialize ....
    // Define a better ressources management and initialization
    m_files.push_back( globjects::File::create( "Shaders/Helpers.glsl" ) );
    m_files.push_back( globjects::File::create( "Shaders/Structs.glsl" ) );
    m_files.push_back( globjects::File::create( "Shaders/Tonemap.glsl" ) );
    m_files.push_back( globjects::File::create( "Shaders/LightingFunctions.glsl" ) );

    m_namedStrings.push_back( globjects::NamedString::create( "/Helpers.glsl", m_files[0].get() ) );
    m_namedStrings.push_back( globjects::NamedString::create( "/Structs.glsl", m_files[1].get() ) );
    m_namedStrings.push_back( globjects::NamedString::create( "/Tonemap.glsl", m_files[2].get() ) );
    m_namedStrings.push_back(
        globjects::NamedString::create( "/LightingFunctions.glsl", m_files[3].get() ) );

    m_files.push_back( globjects::File::create( "Shaders/Transform/TransformStructs.glsl" ) );
    m_namedStrings.push_back(
        globjects::NamedString::create( "/TransformStructs.glsl", m_files[4].get() ) );

    m_files.push_back(
        globjects::File::create( "Shaders/Materials/BlinnPhong/BlinnPhongMaterial.glsl" ) );
    m_namedStrings.push_back(
        globjects::NamedString::create( "/BlinnPhongMaterial.glsl", m_files[5].get() ) );

    m_files.push_back( globjects::File::create( "Shaders/Lights/DefaultLight.glsl" ) );
    m_namedStrings.push_back(
        globjects::NamedString::create( "/DefaultLight.glsl", m_files[6].get() ) );

    m_defaultShaderProgram =
        addShaderProgram( { {"Default Program"}, m_defaultVsName, m_defaultFsName } );
}

void ShaderProgramManager::addNamedString( const std::string& includepath,
                                           const std::string& realfile ) {
    LOG( logINFO ) << "Inserting named string : " << includepath << " --> " << realfile;
    m_files.push_back( globjects::File::create( realfile ) );
    m_namedStrings.push_back(
        globjects::NamedString::create( includepath, m_files[m_files.size() - 1].get() ) );
}

void ShaderProgramManager::reloadNamedString() {
    auto numNamedString = m_namedStrings.size();
    for ( auto i = 0; i < numNamedString; ++i )
    {
        m_files[i]->reload();
        std::string id = m_namedStrings[i]->name();
        m_namedStrings[i].reset( nullptr );
        m_namedStrings[i] = globjects::NamedString::create( id, m_files[i].get() );
    }
}

const ShaderProgram* ShaderProgramManager::addShaderProgram( const ShaderConfiguration& config ) {
    auto found = m_shaderPrograms.find( config );

    if ( found != m_shaderPrograms.end() )
    {
        return found->second.get();
    }

    // Try to load the shader
    auto prog = Core::make_shared<ShaderProgram>( config );

    if ( prog->getProgramObject()->isLinked() )
    {
        insertShader( config, prog );
        return prog.get();
    } else
    {
        LOG( logERROR ) << "Error occurred while loading shader program "
                        << config.m_name.c_str()
                        << ":\nDefault shader program used instead.";

        // insert in the failed shaders list
        m_shaderFailedConfs.push_back( config );

        return getDefaultShaderProgram();
    }
}

const ShaderProgram* ShaderProgramManager::getShaderProgram( const std::string& id ) {
    auto found = m_shaderProgramIds.find( id );

    if ( found != m_shaderProgramIds.end() )
    {
        return getShaderProgram( found->second );
    }
    return nullptr;
}

const ShaderProgram* ShaderProgramManager::getShaderProgram( const ShaderConfiguration& config ) {
    return addShaderProgram( config );
}

void ShaderProgramManager::reloadAllShaderPrograms() {
    // update the include registry
    reloadNamedString();

    // For each shader in the map
    for ( auto& shader : m_shaderPrograms )
    {
        shader.second->reload();
    }

    // and also try the failed ones
    reloadNotCompiledShaderPrograms();
}

void ShaderProgramManager::reloadNotCompiledShaderPrograms() {
    // for each shader in the failed map, try to reload
    for ( const auto & conf : m_shaderFailedConfs )
    {
        auto prog = Core::make_shared<ShaderProgram>( conf );

        if ( prog->getProgramObject()->isValid() )
        {
            insertShader( conf, prog );
            // m_shaderFailedConfs.erase(conf);
        }
    }
}

const ShaderProgram* ShaderProgramManager::getDefaultShaderProgram() const {
    return m_defaultShaderProgram;
}

void ShaderProgramManager::insertShader( const ShaderConfiguration& config,
                                         const ShaderProgramPtr& shader ) {
    m_shaderProgramIds.insert( {config.m_name, config} );
    m_shaderPrograms.insert( {config, shader} );
}

RA_SINGLETON_IMPLEMENTATION( ShaderProgramManager );
} // namespace Engine
} // namespace Ra
