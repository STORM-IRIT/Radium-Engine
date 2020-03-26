#include <GuiBase/Viewer/CameraManipulator.hpp>

#include <Core/Math/Math.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Light/Light.hpp>

#include <GuiBase/Viewer/Viewer.hpp>

namespace Ra {

using namespace Core::Utils; // log

Gui::CameraManipulator::CameraManipulator( const CameraManipulator& other ) :
    QObject(),
    m_cameraSensitivity( other.m_cameraSensitivity ),
    m_quickCameraModifier( other.m_quickCameraModifier ),
    m_wheelSpeedModifier( other.m_wheelSpeedModifier ),
    m_targetedAabbVolume( other.m_targetedAabbVolume ),
    m_mapCameraBahaviourToAabb( other.m_mapCameraBahaviourToAabb ),
    m_target( other.m_target ),
    m_camera( other.m_camera ),
    m_light( other.m_light ) {}

Gui::CameraManipulator::CameraManipulator( uint width, uint height ) :
    m_cameraSensitivity( 1.0_ra ),
    m_quickCameraModifier( 1._ra ),
    m_wheelSpeedModifier( 0.02_ra ),
    m_targetedAabbVolume( 0.0_ra ),
    m_mapCameraBahaviourToAabb( false ),
    m_target( 0_ra, 0_ra, 0_ra ),
    m_camera( nullptr ),
    m_light( nullptr ) {
    auto it = std::find_if(
        Engine::SystemEntity::getInstance()->getComponents().cbegin(),
        Engine::SystemEntity::getInstance()->getComponents().cend(),
        []( const auto& c ) { return c->getName().compare( "CAMERA_DEFAULT" ) == 0; } );
    if ( it != Engine::SystemEntity::getInstance()->getComponents().cend() )
    { m_camera = static_cast<Engine::Camera*>( ( *it ).get() ); }
    else
    {
        m_camera = new Engine::Camera( Engine::SystemEntity::getInstance(),
                                       "CAMERA_DEFAULT",
                                       Scalar( height ),
                                       Scalar( width ) );
        m_camera->initialize();
        m_camera->show( false );

        setCameraFovInDegrees( 60.0_ra );
        setCameraZNear( 0.1_ra );
        setCameraZFar( 1000.0_ra );
    }
}

void Gui::CameraManipulator::resetToDefaultCamera() {
    // get parameters from the current camera
    // Thisis awfull and requires that the current camera is still alive ...
    Scalar w = m_camera->getWidth();
    Scalar h = m_camera->getHeight();
    auto it  = std::find_if(
        Engine::SystemEntity::getInstance()->getComponents().cbegin(),
        Engine::SystemEntity::getInstance()->getComponents().cend(),
        []( const auto& c ) { return c->getName().compare( "CAMERA_DEFAULT" ) == 0; } );
    if ( it != Engine::SystemEntity::getInstance()->getComponents().cend() )
    {
        m_camera = static_cast<Engine::Camera*>( ( *it ).get() );
        m_camera->resize( w, h );
        m_camera->show( false );
    }
    else
    {
        LOG( logWARNING )
            << "A living camera is required. The application might now behave unexpectedly.";
    }
}

Gui::CameraManipulator::~CameraManipulator() {}

void Gui::CameraManipulator::resizeViewport( uint width, uint height ) {
    m_camera->resize( Scalar( width ), Scalar( height ) );
}

Core::Matrix4 Gui::CameraManipulator::getProjMatrix() const {
    return m_camera->getProjMatrix();
}

Core::Matrix4 Gui::CameraManipulator::getViewMatrix() const {
    return m_camera->getViewMatrix();
}

void Gui::CameraManipulator::setCameraSensitivity( Scalar sensitivity ) {
    m_cameraSensitivity = sensitivity;
}

void Gui::CameraManipulator::setCameraFov( Scalar fov ) {
    m_camera->setFOV( fov );
}

void Gui::CameraManipulator::setCameraFovInDegrees( Scalar fov ) {
    m_camera->setFOV( fov * Core::Math::toRad );
}

void Gui::CameraManipulator::setCameraZNear( Scalar zNear ) {
    m_camera->setZNear( zNear );
}

void Gui::CameraManipulator::setCameraZFar( Scalar zFar ) {
    m_camera->setZFar( zFar );
}

void Gui::CameraManipulator::mapCameraBehaviourToAabb( const Core::Aabb& aabb ) {
    m_targetedAabb             = aabb;
    m_targetedAabbVolume       = aabb.volume();
    m_mapCameraBahaviourToAabb = true;
}

void Gui::CameraManipulator::unmapCameraBehaviourToAabb() {
    m_mapCameraBahaviourToAabb = false;
}

void Gui::CameraManipulator::attachLight( Engine::Light* light ) {
    m_light = light;
    m_light->setDirection( m_camera->getDirection() );
}

Gui::KeyMappingManager::Context Gui::CameraManipulator::mappingContext() {
    return Gui::KeyMappingManager::Context();
}

const Engine::Camera& Gui::CameraManipulator::getCameraFromViewer( QObject* v ) {
    return *static_cast<Gui::Viewer*>( v )->getCameraManipulator()->getCamera();
}

} // namespace Ra
