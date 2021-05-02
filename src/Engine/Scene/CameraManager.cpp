#include <Core/Asset/Camera.hpp>
#include <Engine/Scene/CameraComponent.hpp>
#include <Engine/Scene/CameraManager.hpp>

#include <Core/Asset/CameraData.hpp>
#include <Core/Asset/FileData.hpp>

#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/FrameInfo.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Scene/ComponentMessenger.hpp>
#include <Engine/Scene/Entity.hpp>
#include <Engine/Scene/SystemDisplay.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

using namespace Core::Utils; // log
using namespace Core::Asset;

Ra::Core::Asset::Camera CameraManager::defaultCamera;

CameraManager::CameraManager() {
    defaultCamera.setFOV( 60.0_ra * Core::Math::toRad );
    defaultCamera.setZNear( 0.1_ra );
    defaultCamera.setZFar( 1000.0_ra );
}

void CameraManager::initialize() {

    if ( count() > 0 )
    {
        LOG( logDEBUG ) << "CameraManager seems to be already initialized, do nothing.";
        return;
    }

    auto comp = new Engine::Scene::CameraComponent( Engine::Scene::SystemEntity::getInstance(),
                                                    "CAMERA_DEFAULT" );
    comp->initialize();
    *comp->getCamera() = defaultCamera;
    addCamera( comp );

    /// TMP FOR TESTS
    comp = new Engine::Scene::CameraComponent( Engine::Scene::SystemEntity::getInstance(),
                                               "CAMERA_DEFAULT60" );
    comp->initialize();
    *comp->getCamera() = defaultCamera;
    addCamera( comp );

    comp = new Engine::Scene::CameraComponent( Engine::Scene::SystemEntity::getInstance(),
                                               "CAMERA_DEFAULT120" );
    comp->initialize();
    *comp->getCamera() = defaultCamera;
    comp->getCamera()->setFOV( 120 * Core::Math::toRad );
    comp->getCamera()->setPosition( {1_ra, 1_ra, 1_ra} );
    comp->getCamera()->setViewport( 100, 10 );

    addCamera( comp );
}

void CameraManager::activate( Core::Utils::Index index ) {

    if ( index.isInvalid() || index > count() )
    {
        LOG( logDEBUG ) << "Try to activate camera with an invalid/out of bound index. Ignored.";
        return;
    }
    // save current size
    auto width                   = getCamera( 0 )->getCamera()->getWidth();
    auto height                  = getCamera( 0 )->getCamera()->getHeight();
    *getCamera( 0 )->getCamera() = *getCamera( index )->getCamera();
    getCamera( 0 )->getCamera()->setViewport( width, height );
    getCamera( 0 )->getCamera()->updateProjMatrix();
}

Ra::Core::Utils::Index CameraManager::getCameraIndex( const CameraComponent* cam ) {
    for ( size_t i = 0; i < m_data->size(); ++i )
    {
        if ( cam == ( *m_data )[i] ) return i;
    }
    return {};
}

size_t CameraManager::count() const {
    return m_data->size();
}

void CameraManager::generateTasks( Core::TaskQueue* taskQueue,
                                   const Engine::FrameInfo& /*frameInfo*/ ) {

    class RoUpdater : public Ra::Core::Task
    {
      public:
        void process() override { m_camera->updateTransform(); }
        std::string getName() const override { return "camera updater"; }
        CameraComponent* m_camera;
    };

    // only update visible components.
    for ( size_t i = 0; i < m_data->size(); ++i )
    {
        auto comp = ( *m_data )[i];
        auto ro   = comp->getRenderObject();
        if ( ro->isVisible() )
        {
            auto updater      = new RoUpdater();
            updater->m_camera = comp;
            taskQueue->registerTask( updater );
        }
    }
}

void CameraManager::handleAssetLoading( Entity* entity, const FileData* filedata ) {
    std::vector<CameraData*> cameraData = filedata->getCameraData();
    uint id                             = 0;
    for ( const auto& data : cameraData )
    {
        std::string componentName = "CAMERA_" + entity->getName() + std::to_string( id++ );
        auto comp                 = new CameraComponent( entity, componentName, 100, 100 );
        switch ( data->getType() )
        {
        case CameraData::ORTHOGRAPHIC: {
            comp->setType( Ra::Core::Asset::Camera::ProjType::ORTHOGRAPHIC );
            break;
        }
        case CameraData::PERSPECTIVE: {
            comp->setType( Ra::Core::Asset::Camera::ProjType::PERSPECTIVE );
            break;
        }
        }
        comp->getCamera()->setFrame( Core::Transform( data->getFrame() ) );
        if ( data->getType() == CameraData::CameraType::ORTHOGRAPHIC )
            comp->getCamera()->setType( Camera::ProjType::ORTHOGRAPHIC );
        else
            comp->getCamera()->setType( Camera::ProjType::PERSPECTIVE );
        comp->getCamera()->setFOV( data->getFov() );
        comp->getCamera()->setZNear( data->getZNear() );
        comp->getCamera()->setZFar( data->getZFar() );
        comp->getCamera()->setZoomFactor( data->getZoomFactor() );

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
