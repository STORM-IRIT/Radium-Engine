#include <GuiBase/Viewer/FlightModeCamera.hpp>

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

#define KMA_VALUE( XX ) Gui::KeyMappingManager::KeyMappingAction Gui::FlightModeCamera::XX;
KeyMappingFlightCamera;
#undef KMA_VALUE

void Gui::FlightModeCamera::configureKeyMapping_impl() {

    m_keyMappingContext =
        Gui::KeyMappingManager::getInstance()->getContext( "FlightCameraContext" );
    if ( m_keyMappingContext.isInvalid() )
    {
        LOG( Ra::Core::Utils::logINFO )
            << "FlightCameraContext not defined (maybe the configuration file do not contains it)";
        LOG( Ra::Core::Utils::logERROR ) << "FlightCameraContext all keymapping invalide !";
        return;
    }

#define KMA_VALUE( XX ) \
    XX = Gui::KeyMappingManager::getInstance()->getActionIndex( m_keyMappingContext, #XX );
    KeyMappingFlightCamera
#undef KMA_VALUE
}

KeyMappingManager::Listener Gui::FlightModeCamera::mappingConfigurationCallback() {
    return Gui::FlightModeCamera::configureKeyMapping;
}

Gui::FlightModeCamera::FlightModeCamera( uint width, uint height ) :
    CameraInterface( width, height ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    resetCamera();
}

Gui::FlightModeCamera::FlightModeCamera( const CameraInterface* other ) :
    CameraInterface( other ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    m_flightSpeed = ( m_target - m_camera->getPosition() ).norm() / 10_ra;
}

Gui::FlightModeCamera::~FlightModeCamera() = default;

void Gui::FlightModeCamera::resetCamera() {
    m_camera->setFrame( Core::Transform::Identity() );
    m_camera->setPosition( Core::Vector3( 0, 0, 1 ) );

    m_target      = m_camera->getPosition() + 2 * m_camera->getDirection().normalized();
    m_flightSpeed = 0.2;

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );
}

bool Gui::FlightModeCamera::handleMousePressEvent( QMouseEvent* event,
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

bool Gui::FlightModeCamera::handleMouseMoveEvent( QMouseEvent* event,
                                                  const Qt::MouseButtons& buttons,
                                                  const Qt::KeyboardModifiers& modifiers,
                                                  int /*key*/ ) {

    // auto action = KeyMappingManager::getInstance()->getAction( context, buttons, modifiers, key
    // );

    Scalar dx = ( event->pos().x() - m_lastMouseX ) / m_camera->getWidth();
    Scalar dy = ( event->pos().y() - m_lastMouseY ) / m_camera->getHeight();

    if ( event->modifiers().testFlag( Qt::AltModifier ) ) { m_quickCameraModifier = 10.0; }
    else
    { m_quickCameraModifier = 2.0; }

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

bool Gui::FlightModeCamera::handleMouseReleaseEvent( QMouseEvent* /*event*/ ) {
    m_cameraRotateMode    = false;
    m_cameraPanMode       = false;
    m_cameraZoomMode      = false;
    m_quickCameraModifier = 1.0;

    return true;
}

bool Gui::FlightModeCamera::handleWheelEvent( QWheelEvent* event ) {

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

void Gui::FlightModeCamera::toggleRotateAround() {
    m_rotateAround = !m_rotateAround;
}

bool Gui::FlightModeCamera::handleKeyPressEvent(
    QKeyEvent* /*event*/,
    const KeyMappingManager::KeyMappingAction& action ) {

    if ( action == FLIGHTMODECAMERA_ROTATE_AROUND )
    {
        m_rotateAround = !m_rotateAround;
        return true;
    }

    return false;
}

bool Gui::FlightModeCamera::handleKeyReleaseEvent( QKeyEvent* /*e*/ ) {
    return false;
}

void Gui::FlightModeCamera::setCamera( Engine::Camera* camera ) {

    if ( !camera ) return;
    camera->resize( m_camera->getWidth(), m_camera->getHeight() );
    m_camera = camera;
    m_target = m_camera->getPosition() + 2 * m_camera->getDirection().normalized();

    m_camera->show( false );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

void Gui::FlightModeCamera::setCameraPosition( const Core::Vector3& position ) {
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

void Gui::FlightModeCamera::setCameraTarget( const Core::Vector3& target ) {
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

void Gui::FlightModeCamera::fitScene( const Core::Aabb& aabb ) {
    resetCamera();

    Scalar f = m_camera->getFOV();
    Scalar a = m_camera->getAspect();

    const Scalar r = ( aabb.max() - aabb.min() ).norm() / 2_ra;
    const Scalar x = r / std::sin( f / 2_ra );
    const Scalar y = r / std::sin( f * a / 2_ra );
    Scalar d       = std::max( std::max( x, y ), 0.001_ra );

    m_camera->setPosition(
        Core::Vector3( aabb.center().x(), aabb.center().y(), aabb.center().z() + d ) );
    m_camera->setDirection( Core::Vector3( 0, 0, -1 ) );
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

void Gui::FlightModeCamera::handleCameraRotate( Scalar dx, Scalar dy ) {
    Scalar dphi   = dx * m_cameraSensitivity * m_quickCameraModifier;
    Scalar dtheta = -dy * m_cameraSensitivity * m_quickCameraModifier;
    Core::Transform R( Core::Transform::Identity() );
    if ( std::abs( dphi ) > std::abs( dtheta ) )
    { R = Core::AngleAxis( -dphi, m_camera->getUpVector().normalized() ); }
    else
    { R = Core::AngleAxis( -dtheta, -m_camera->getRightVector().normalized() ); }
    Scalar d = ( m_target - m_camera->getPosition() ).norm();
    m_camera->applyTransform( R );
    m_target = m_camera->getPosition() + d * m_camera->getDirection();
}

void Gui::FlightModeCamera::handleCameraPan( Scalar dx, Scalar dy ) {
    Scalar x = dx * m_cameraSensitivity * m_quickCameraModifier;
    Scalar y = dy * m_cameraSensitivity * m_quickCameraModifier;
    // Move camera and trackball center, keep the distance to the center
    Core::Vector3 R = -m_camera->getRightVector();
    Core::Vector3 U = m_camera->getUpVector();

    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = x * R + y * U;
    T.translate( t );

    m_camera->applyTransform( T );
    m_target += t;
}

void Gui::FlightModeCamera::handleCameraZoom( Scalar dx, Scalar dy ) {
    handleCameraZoom( Ra::Core::Math::sign( dx ) * ( std::abs( dx ) + std::abs( dy ) ) );
}

void Gui::FlightModeCamera::handleCameraZoom( Scalar z ) {
    auto y = m_flightSpeed * z * m_cameraSensitivity * m_quickCameraModifier;
    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = y * m_camera->getDirection();
    T.translate( t );
    m_camera->applyTransform( T );
    m_target += t;
}

} // namespace Gui
} // namespace Ra
