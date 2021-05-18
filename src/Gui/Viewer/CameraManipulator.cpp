#include <Gui/Viewer/CameraManipulator.hpp>

#include <Core/Math/Math.hpp>
#include <Engine/Scene/CameraManager.hpp>
#include <Engine/Scene/Light.hpp>

#include <Core/Asset/Camera.hpp>
#include <Gui/Viewer/Viewer.hpp>

namespace Ra {
namespace Gui {
using namespace Core::Utils; // log

CameraManipulator::CameraManipulator( const CameraManipulator& other ) :
    QObject(),
    m_cameraSensitivity( other.m_cameraSensitivity ),
    m_quickCameraModifier( other.m_quickCameraModifier ),
    m_wheelSpeedModifier( other.m_wheelSpeedModifier ),
    m_targetedAabbVolume( other.m_targetedAabbVolume ),
    m_mapCameraBahaviourToAabb( other.m_mapCameraBahaviourToAabb ),
    m_target( other.m_target ),
    m_camera( other.m_camera ),
    m_light( other.m_light ) {}

CameraManipulator::CameraManipulator() {

    auto cameraManager = static_cast<Ra::Engine::Scene::CameraManager*>(
        Engine::RadiumEngine::getInstance()->getSystem( "DefaultCameraManager" ) );
    m_camera = cameraManager->getActiveCamera();
}

CameraManipulator::~CameraManipulator() {}

void CameraManipulator::setCameraSensitivity( Scalar sensitivity ) {
    m_cameraSensitivity = sensitivity;
}

void CameraManipulator::setCameraFov( Scalar fov ) {
    m_camera->setFOV( fov );
}

void CameraManipulator::setCameraFovInDegrees( Scalar fov ) {
    m_camera->setFOV( fov * Core::Math::toRad );
}

void CameraManipulator::setCameraZNear( Scalar zNear ) {
    m_camera->setZNear( zNear );
}

void CameraManipulator::setCameraZFar( Scalar zFar ) {
    m_camera->setZFar( zFar );
}

void CameraManipulator::updateCamera() {}

void CameraManipulator::mapCameraBehaviourToAabb( const Core::Aabb& aabb ) {
    m_targetedAabb             = aabb;
    m_targetedAabbVolume       = aabb.volume();
    m_mapCameraBahaviourToAabb = true;
}

void CameraManipulator::unmapCameraBehaviourToAabb() {
    m_mapCameraBahaviourToAabb = false;
}

void CameraManipulator::attachLight( Engine::Scene::Light* light ) {
    m_light = light;
    m_light->setDirection( m_camera->getDirection() );
}

KeyMappingManager::Context Gui::CameraManipulator::mappingContext() {
    return Gui::KeyMappingManager::Context();
}

} // namespace Gui
} // namespace Ra
