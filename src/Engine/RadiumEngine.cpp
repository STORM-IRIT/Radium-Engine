#include <Engine/RadiumEngine.hpp>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <mutex>
#include <streambuf>
#include <string>
#include <thread>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/FileLoaderInterface.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Utils/StringUtils.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/LambertianMaterial.hpp>
#include <Engine/Renderer/Material/MaterialConverters.hpp>
#include <Engine/Renderer/Material/PlainMaterial.hpp>
#include <Engine/Renderer/Material/VolumetricMaterial.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/System/System.hpp>

namespace Ra {
namespace Engine {

using namespace Core::Utils; // log
using namespace Core::Asset;

RadiumEngine::RadiumEngine() = default;

RadiumEngine::~RadiumEngine() = default;

void RadiumEngine::initialize() {
    LOG( logINFO ) << "*** Radium Engine ***";
    m_resourcesRootDir    = {Core::Resources::getRadiumResourcesDir()};
    m_signalManager       = std::make_unique<SignalManager>();
    m_entityManager       = std::make_unique<EntityManager>();
    m_renderObjectManager = std::make_unique<RenderObjectManager>();
    m_loadedFile.reset();
    ComponentMessenger::createInstance();
    m_loadingState = false;
}

void RadiumEngine::registerDefaultPrograms() {
    auto shaderProgramManager = ShaderProgramManager::getInstance();
    CORE_ASSERT( shaderProgramManager != nullptr,
                 "ShaderProgramManager needs to be created first" );

    // Create named strings which correspond to shader files that you want to use in shaders's
    // includes. NOTE: if you want to add a named string to handle a new shader include file, be
    // SURE that the name (first parameter) begin with a "/", otherwise it won't work !
    // Radium V2 : are these initialization required here ? They will be better in
    // Engine::Initialize .... Define a better ressources management and initialization
    /* Default definiton of a transformation matrices struct */
    shaderProgramManager->addNamedString(
        "/TransformStructs.glsl", m_resourcesRootDir + "Shaders/Transform/TransformStructs.glsl" );
    shaderProgramManager->addNamedString( "/DefaultLight.glsl",
                                          m_resourcesRootDir + "Shaders/Lights/DefaultLight.glsl" );

    // VertexAttribInterface :add this name string so that each material could include the same code
    ShaderProgramManager::getInstance()->addNamedString(
        "/VertexAttribInterface.frag.glsl",
        m_resourcesRootDir + "Shaders/Materials/VertexAttribInterface.frag.glsl" );

    // Engine support some built-in materials. Register here
    /// @todo find a way to integrate "Line" material into Radium Material System
    ShaderConfiguration lConfig( "Lines" );
    lConfig.addShader( ShaderType_VERTEX, m_resourcesRootDir + "Shaders/Lines/Lines.vert.glsl" );
    lConfig.addShader( ShaderType_FRAGMENT, m_resourcesRootDir + "Shaders/Lines/Lines.frag.glsl" );
    ShaderConfigurationFactory::addConfiguration( lConfig );

    // Plain is flat or diffuse
    PlainMaterial::registerMaterial();
    BlinnPhongMaterial::registerMaterial();
    LambertianMaterial::registerMaterial();
    VolumetricMaterial::registerMaterial();
}

void RadiumEngine::cleanup() {
    PlainMaterial::unregisterMaterial();
    BlinnPhongMaterial::unregisterMaterial();
    LambertianMaterial::unregisterMaterial();
    VolumetricMaterial::unregisterMaterial();
    m_signalManager->setOn( false );
    m_entityManager.reset();
    m_renderObjectManager.reset();
    m_loadedFile.reset();

    for ( auto& system : m_systems )
    {
        system.second.reset();
    }

    ComponentMessenger::destroyInstance();
    ShaderProgramManager::destroyInstance();
    m_loadingState = false;
}

void RadiumEngine::endFrameSync() {
    m_entityManager->swapBuffers();
    m_signalManager->fireFrameEnded();
}

void RadiumEngine::getTasks( Core::TaskQueue* taskQueue, Scalar dt ) {
    static uint frameCounter = 0;
    FrameInfo frameInfo{dt, frameCounter++};
    for ( auto& syst : m_systems )
    {
        syst.second->generateTasks( taskQueue, frameInfo );
    }
}

bool RadiumEngine::registerSystem( const std::string& name, System* system, int priority ) {
    if ( findSystem( name ) != m_systems.end() )
    {
        LOG( logWARNING ) << "Try to add system " << name.c_str()
                          << " multiple time. Keep the already registered one.";
        return false;
    }

    m_systems[std::make_pair( priority, name )] = std::shared_ptr<System>( system );
    LOG( logINFO ) << "Loaded : " << name;
    return true;
}

System* RadiumEngine::getSystem( const std::string& system ) const {
    System* sys = nullptr;
    auto it     = findSystem( system );

    if ( it != m_systems.end() ) { sys = it->second.get(); }

    return sys;
}

Displayable* RadiumEngine::getMesh( const std::string& entityName,
                                    const std::string& componentName,
                                    const std::string& roName ) const {

    // 1) Get entity
    if ( m_entityManager->entityExists( entityName ) )
    {
        auto e = m_entityManager->getEntity( entityName );

        // 2) Get component
        const auto c = e->getComponent( componentName );

        if ( c != nullptr && !c->m_renderObjects.empty() )
        {
            // 3) Get RO
            if ( roName.empty() )
            {
                return m_renderObjectManager->getRenderObject( c->m_renderObjects.front() )
                    ->getMesh()
                    .get();
            }
            else
            {
                for ( const auto& idx : c->m_renderObjects )
                {
                    const auto& ro = m_renderObjectManager->getRenderObject( idx );
                    if ( ro->getName() == roName ) { return ro->getMesh().get(); }
                }
            }
        }
    }
    return nullptr;
}

bool RadiumEngine::loadFile( const std::string& filename ) {
    releaseFile();

    std::string extension = Core::Utils::getFileExt( filename );

    for ( auto& l : m_fileLoaders )
    {
        if ( l->handleFileExtension( extension ) )
        {
            FileData* data = l->loadFile( filename );
            if ( data != nullptr )
            {
                m_loadedFile.reset( data );
                break;
            }
        }
    }

    if ( m_loadedFile == nullptr )
    {
        LOG( logERROR ) << "There is no loader to handle \"" << extension
                        << "\" extension ! File can't be loaded.";

        return false;
    }

    std::string entityName = Core::Utils::getBaseName( filename, false );

    Entity* entity = m_entityManager->createEntity( entityName );

    for ( auto& system : m_systems )
    {
        system.second->handleAssetLoading( entity, m_loadedFile.get() );
    }

    if ( !entity->getComponents().empty() )
    {
        for ( auto& comp : entity->getComponents() )
        {
            comp->initialize();
        }
    }
    else
    {
        LOG( logWARNING ) << "File \"" << filename << "\" has no usable data. Deleting entity...";
        m_entityManager->removeEntity( entity );
    }
    m_loadingState = true;
    return true;
}

void RadiumEngine::releaseFile() {
    m_loadedFile.reset( nullptr );
    m_loadingState = false;
}

RenderObjectManager* RadiumEngine::getRenderObjectManager() const {
    return m_renderObjectManager.get();
}

EntityManager* RadiumEngine::getEntityManager() const {
    return m_entityManager.get();
}

SignalManager* RadiumEngine::getSignalManager() const {
    return m_signalManager.get();
}

void RadiumEngine::registerFileLoader( std::shared_ptr<FileLoaderInterface> fileLoader ) {
    m_fileLoaders.push_back( fileLoader );
}

const std::vector<std::shared_ptr<FileLoaderInterface>>& RadiumEngine::getFileLoaders() const {
    return m_fileLoaders;
}

RA_SINGLETON_IMPLEMENTATION( RadiumEngine );

const FileData& RadiumEngine::getFileData() const {
    CORE_ASSERT( m_loadingState, "Access to file content is only available at loading time." );
    return *( m_loadedFile.get() );
}

RadiumEngine::SystemContainer::const_iterator
RadiumEngine::findSystem( const std::string& name ) const {
    return std::find_if( m_systems.cbegin(), m_systems.cend(), [&name]( const auto& el ) {
        return el.first.second == name;
    } );
}

RadiumEngine::SystemContainer::iterator RadiumEngine::findSystem( const std::string& name ) {
    return std::find_if( m_systems.begin(), m_systems.end(), [&name]( const auto& el ) {
        return el.first.second == name;
    } );
}

Core::Aabb RadiumEngine::computeSceneAabb() const {

    Core::Aabb aabb;

    const auto& systemEntity = Engine::SystemEntity::getInstance();
    auto entities            = m_entityManager->getEntities();
    for ( const auto& entity : entities )
    {
        if ( entity != systemEntity ) aabb.extend( entity->computeAabb() );
    }
    return aabb;
}

} // namespace Engine
} // namespace Ra
