#include <GuiBase/Viewer/FlightCameraManipulator.hpp>

#include <Core/Math/Math.hpp>
#include <Core/Utils/Log.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <GuiBase/Utils/Keyboard.hpp>

#include <GuiBase/Utils/KeyMappingManager.hpp>

#include <QApplication>
#include <QMessageBox>
#include <algorithm>
#include <iostream>

namespace Ra {
namespace Gui {
using Core::Math::Pi;

#define KMA_VALUE( XX ) Gui::KeyMappingManager::KeyMappingAction Gui::FlightCameraManipulator::XX;
KeyMappingFlightManipulator
#undef KMA_VALUE

    void
    Gui::FlightCameraManipulator::configureKeyMapping_impl() {

    m_keyMappingContext =
        Gui::KeyMappingManager::getInstance()->getContext( "FlightManipulatorContext" );
    if ( m_keyMappingContext.isInvalid() )
    {
        LOG( Ra::Core::Utils::logWARNING ) << "FlightManipulatorContext not defined (maybe the "
                                              "configuration file do not contains it). Adding "
                                              "default configuration for FlightManipulatorContext.";

        Gui::KeyMappingManager::getInstance()->addAction(
            "FlightManipulatorContext", "", "", "LeftButton", "", "FLIGHTMODECAMERA_ROTATE" );
        Gui::KeyMappingManager::getInstance()->addAction( "FlightManipulatorContext",
                                                          "",
                                                          "ShiftModifier",
                                                          "LeftButton",
                                                          "",
                                                          "FLIGHTMODECAMERA_PAN" );
        Gui::KeyMappingManager::getInstance()->addAction( "FlightManipulatorContext",
                                                          "",
                                                          "ControlModifier",
                                                          "LeftButton",
                                                          "",
                                                          "FLIGHTMODECAMERA_ZOOM" );
        Gui::KeyMappingManager::getInstance()->addAction(
            "FlightManipulatorContext", "Key_A", "", "", "", "FLIGHTMODECAMERA_ROTATE_AROUND" );
        m_keyMappingContext =
            Gui::KeyMappingManager::getInstance()->getContext( "FlightManipulatorContext" );
    }

#define KMA_VALUE( XX ) \
    XX = Gui::KeyMappingManager::getInstance()->getActionIndex( m_keyMappingContext, #XX );
    KeyMappingFlightManipulator
#undef KMA_VALUE
}

KeyMappingManager::Context Gui::FlightCameraManipulator::mappingContext() {
    return m_keyMappingContext;
}

Gui::FlightCameraManipulator::FlightCameraManipulator( uint width, uint height ) :
    CameraManipulator( width, height ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    resetCamera();
}

Gui::FlightCameraManipulator::FlightCameraManipulator( const FlightCameraManipulator& other ) :
    CameraManipulator( other ),
    m_rotateAround( other.m_rotateAround ),
    m_cameraRotateMode( other.m_cameraRotateMode ),
    m_cameraPanMode( other.m_cameraPanMode ),
    m_cameraZoomMode( other.m_cameraZoomMode ),
    m_fixUpVector( other.m_fixUpVector ),
    m_flightSpeed( other.m_flightSpeed ) {}

Gui::FlightCameraManipulator::FlightCameraManipulator( const CameraManipulator& other ) :
    CameraManipulator( other ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    m_flightSpeed = ( m_target - m_camera->getPosition() ).norm() / 10_ra;
    initializeFixedUpVector();
}

Gui::FlightCameraManipulator::~FlightCameraManipulator() = default;

void Gui::FlightCameraManipulator::resetCamera() {
    m_camera->setFrame( Core::Transform::Identity() );
    m_camera->setPosition( Core::Vector3( 0, 0, 1 ) );
    initializeFixedUpVector();

    m_target      = m_camera->getPosition() + 2_ra * m_camera->getDirection().normalized();
    m_flightSpeed = 0.2_ra;

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );
}

bool Gui::FlightCameraManipulator::handleMousePressEvent( QMouseEvent* event,
                                                          const Qt::MouseButtons& buttons,
                                                          const Qt::KeyboardModifiers& modifiers,
                                                          int key ) {
    bool handled = false;
    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    auto action = KeyMappingManager::getInstance()->getAction(
        m_keyMappingContext, buttons, modifiers, key, false );

    if ( action == FLIGHTMODECAMERA_ROTATE )
    {
        m_cameraRotateMode = true;
        handled            = true;
    }
    if ( action == FLIGHTMODECAMERA_PAN )
    {
        m_cameraPanMode = true;
        handled         = true;
    }
    if ( action == FLIGHTMODECAMERA_ZOOM )
    {
        m_cameraZoomMode = true;
        handled          = true;
    }

    return handled;
}

bool Gui::FlightCameraManipulator::handleMouseMoveEvent( QMouseEvent* event,
                                                         const Qt::MouseButtons& /*buttons*/,
                                                         const Qt::KeyboardModifiers& /*modifiers*/,
                                                         int /*key*/ ) {

    // auto action = KeyMappingManager::getInstance()->getAction( context, buttons, modifiers, key
    // );

    Scalar dx = ( event->pos().x() - m_lastMouseX ) / m_camera->getWidth();
    Scalar dy = ( event->pos().y() - m_lastMouseY ) / m_camera->getHeight();

    if ( event->modifiers().testFlag( Qt::AltModifier ) ) { m_quickCameraModifier = 10.0_ra; }
    else
    { m_quickCameraModifier = 2.0_ra; }

    if ( m_cameraRotateMode ) { handleCameraRotate( dx, dy ); }

    if ( m_cameraPanMode ) { handleCameraPan( dx, dy ); }

    if ( m_cameraZoomMode ) { handleCameraZoom( dx, dy ); }

    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );

    return m_cameraRotateMode || m_cameraPanMode || m_cameraZoomMode;
}

bool Gui::FlightCameraManipulator::handleMouseReleaseEvent( QMouseEvent* /*event*/ ) {
    m_cameraRotateMode    = false;
    m_cameraPanMode       = false;
    m_cameraZoomMode      = false;
    m_quickCameraModifier = 1.0_ra;

    return true;
}

bool Gui::FlightCameraManipulator::handleWheelEvent( QWheelEvent* event ) {

    handleCameraZoom( ( event->angleDelta().y() + event->angleDelta().x() ) *
                      m_wheelSpeedModifier );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraPositionChanged( m_camera->getPosition() );

    return true;
}

void Gui::FlightCameraManipulator::toggleRotateAround() {
    m_rotateAround = !m_rotateAround;
}

bool Gui::FlightCameraManipulator::handleKeyPressEvent(
    QKeyEvent* /*event*/,
    const KeyMappingManager::KeyMappingAction& action ) {

    if ( action == FLIGHTMODECAMERA_ROTATE_AROUND )
    {
        m_rotateAround = !m_rotateAround;
        return true;
    }

    return false;
}

bool Gui::FlightCameraManipulator::handleKeyReleaseEvent( QKeyEvent* /*e*/ ) {
    return false;
}

void Gui::FlightCameraManipulator::setCamera( Engine::Camera* camera ) {

    if ( !camera ) return;
    camera->resize( m_camera->getWidth(), m_camera->getHeight() );
    m_camera = camera;
    m_target = m_camera->getPosition() + 2_ra * m_camera->getDirection().normalized();
    initializeFixedUpVector();

    m_camera->show( false );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

void Gui::FlightCameraManipulator::setCameraPosition( const Core::Vector3& position ) {
    if ( position == m_target )
    {
        QMessageBox::warning( nullptr, "Error", "Position cannot be set to target point" );
        return;
    }
    m_camera->setPosition( position );
    m_camera->setDirection( ( m_target - position ).normalized() );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraPositionChanged( m_camera->getPosition() );
}

void Gui::FlightCameraManipulator::setCameraTarget( const Core::Vector3& target ) {
    if ( m_camera->getPosition() == m_target )
    {
        QMessageBox::warning( nullptr, "Error", "Target cannot be set to current camera position" );
        return;
    }

    m_target = target;
    m_camera->setDirection( ( target - m_camera->getPosition() ).normalized() );

    if ( m_light != nullptr ) { m_light->setDirection( m_camera->getDirection() ); }

    emit cameraTargetChanged( m_target );
}

void Gui::FlightCameraManipulator::fitScene( const Core::Aabb& aabb ) {
    resetCamera();

    Scalar f = m_camera->getFOV();
    Scalar a = m_camera->getAspect();

    const Scalar r = ( aabb.max() - aabb.min() ).norm() / 2_ra;
    const Scalar x = r / std::sin( f / 2_ra );
    const Scalar y = r / std::sin( f * a / 2_ra );
    Scalar d       = std::max( std::max( x, y ), 0.001_ra );

    m_camera->setPosition(
        Core::Vector3( aabb.center().x(), aabb.center().y(), aabb.center().z() + d ) );
    m_camera->setDirection( Core::Vector3( 0_ra, 0_ra, -1_ra ) );
    initializeFixedUpVector();
    m_target = aabb.center();

    m_flightSpeed = d / 10_ra;

    Scalar zfar = std::max( d + ( aabb.max().z() - aabb.min().z() ) * 2_ra, m_camera->getZFar() );
    m_camera->setZFar( zfar );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );
}

void Gui::FlightCameraManipulator::handleCameraRotate( Scalar dx, Scalar dy ) {
    Scalar dphi   = dx * m_cameraSensitivity * m_quickCameraModifier;
    Scalar dtheta = -dy * m_cameraSensitivity * m_quickCameraModifier;

    Core::Transform R( Core::Transform::Identity() );
    if ( std::abs( dphi ) > std::abs( dtheta ) )
    { R = Core::AngleAxis( -dphi, /*m_camera->getUpVector().normalized()*/ m_fixUpVector ); }
    else
    { R = Core::AngleAxis( -dtheta, -m_camera->getRightVector().normalized() ); }

    Scalar d = ( m_target - m_camera->getPosition() ).norm();

    m_camera->applyTransform( R );
    m_target = m_camera->getPosition() + d * m_camera->getDirection();
}

void Gui::FlightCameraManipulator::handleCameraPan( Scalar dx, Scalar dy ) {
    Scalar x = dx * m_cameraSensitivity * m_quickCameraModifier;
    Scalar y = dy * m_cameraSensitivity * m_quickCameraModifier;
    // Move camera and trackball center, keep the distance to the center
    Core::Vector3 R = -m_camera->getRightVector();
    Core::Vector3 U = m_fixUpVector; // m_camera->getUpVector();

    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = x * R + y * U;
    T.translate( t );

    m_camera->applyTransform( T );
    m_target += t;
}

void Gui::FlightCameraManipulator::handleCameraZoom( Scalar dx, Scalar dy ) {
    handleCameraZoom( Ra::Core::Math::sign( dx ) * ( std::abs( dx ) + std::abs( dy ) ) );
}

void Gui::FlightCameraManipulator::handleCameraZoom( Scalar z ) {
    auto y = m_flightSpeed * z * m_cameraSensitivity * m_quickCameraModifier;
    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = y * m_camera->getDirection();
    T.translate( t );
    m_camera->applyTransform( T );
    m_target += t;
}

void Gui::FlightCameraManipulator::initializeFixedUpVector() {
#if 0
    // This is supposed to adapt the up vector to the main up direction but is not really satisfactory.
    const auto& upVector = m_camera->getUpVector();
    if ( std::abs(upVector[0]) >  std::abs(upVector[1]) )
    {
        if (  std::abs(upVector[0]) >  std::abs(upVector[2]) ) { m_fixUpVector = Ra::Core::Vector3( 1, 0, 0 ); }
        else
        { m_fixUpVector = Ra::Core::Vector3( 0, 0, 1 ); }
    }
    else if (  std::abs(upVector[1]) >  std::abs(upVector[2]) )
    { m_fixUpVector = Ra::Core::Vector3( 0, 1, 0 ); }
    else
    { m_fixUpVector = Ra::Core::Vector3( 0, 0, 1 ); }
#endif
    m_fixUpVector = Ra::Core::Vector3( 0_ra, 1_ra, 0_ra );
}

} // namespace Gui
} // namespace Ra
