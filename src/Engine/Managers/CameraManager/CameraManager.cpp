#include <Engine/Managers/CameraManager/CameraManager.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

#include <Core/Asset/CameraData.hpp>
#include <Core/Asset/FileData.hpp>

#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {

using namespace Core::Utils; // log
using namespace Core::Asset;

size_t CameraManager::count() const {
    return m_data->size();
}

void CameraManager::generateTasks( Core::TaskQueue* /*taskQueue*/,
                                   const Engine::FrameInfo& /*frameInfo*/ ) {}

void CameraManager::handleAssetLoading( Entity* entity, const FileData* filedata ) {
    std::vector<CameraData*> cameraData = filedata->getCameraData();
    uint id                             = 0;
    m_data->clear();
    for ( const auto& data : cameraData )
    {
        std::string componentName = "CAMERA_" + entity->getName() + std::to_string( id++ );
        auto comp                 = new Camera( entity, componentName, 100, 100 );
        switch ( data->getType() )
        {
        case CameraData::ORTHOGRAPHIC: {
            comp->setType( Camera::ProjType::ORTHOGRAPHIC );
            break;
        }
        case CameraData::PERSPECTIVE: {
            comp->setType( Camera::ProjType::PERSPECTIVE );
            break;
        }
        }
        comp->setFrame( Core::Transform( data->getFrame() ) );
        comp->setFOV( data->getFov() );
        comp->setZNear( data->getZNear() );
        comp->setZFar( data->getZFar() );
        comp->setZoomFactor( data->getZoomFactor() );

        // comp should be allocated in CameraStorage (well, not sure ...)
        if ( !comp ) continue;

        registerComponent( entity, comp );
    }
    LOG( logINFO ) << "CameraManager : loaded " << count() << " Cameras.";
}

void CameraManager::registerComponent( const Entity* entity, Component* component ) {
    System::registerComponent( entity, component );
    m_data->add( reinterpret_cast<Camera*>( component ) );
}

void CameraManager::unregisterComponent( const Entity* entity, Component* component ) {
    System::unregisterComponent( entity, component );
    m_data->remove( reinterpret_cast<Camera*>( component ) );
}

void CameraManager::unregisterAllComponents( const Entity* entity ) {
    for ( const auto& comp : this->m_components )
    {
        if ( comp.first == entity ) { m_data->remove( reinterpret_cast<Camera*>( comp.second ) ); }
    }
    System::unregisterAllComponents( entity );
}

} // namespace Engine
} // namespace Ra
