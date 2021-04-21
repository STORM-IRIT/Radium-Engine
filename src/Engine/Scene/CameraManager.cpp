#include <Core/Utils/Camera.hpp>
#include <Engine/Scene/CameraComponent.hpp>
#include <Engine/Scene/CameraManager.hpp>

#include <Core/Asset/CameraData.hpp>
#include <Core/Asset/FileData.hpp>

#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/FrameInfo.hpp>
#include <Engine/Scene/ComponentMessenger.hpp>
#include <Engine/Scene/Entity.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

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
        auto comp                 = new CameraComponent( entity, componentName, 100, 100 );
        switch ( data->getType() )
        {
        case CameraData::ORTHOGRAPHIC: {
            comp->setType( Ra::Core::Utils::Camera::ProjType::ORTHOGRAPHIC );
            break;
        }
        case CameraData::PERSPECTIVE: {
            comp->setType( Ra::Core::Utils::Camera::ProjType::PERSPECTIVE );
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
    m_data->add( reinterpret_cast<CameraComponent*>( component ) );
}

void CameraManager::unregisterComponent( const Entity* entity, Component* component ) {
    System::unregisterComponent( entity, component );
    m_data->remove( reinterpret_cast<CameraComponent*>( component ) );
}

void CameraManager::unregisterAllComponents( const Entity* entity ) {
    for ( const auto& comp : this->m_components )
    {
        if ( comp.first == entity )
        { m_data->remove( reinterpret_cast<CameraComponent*>( comp.second ) ); }
    }
    System::unregisterAllComponents( entity );
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
