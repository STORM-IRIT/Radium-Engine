#include <Gui/Viewer/FlightCameraManipulator.hpp>

#include <Core/Asset/Camera.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Utils/Log.hpp>
#include <Engine/Scene/Light.hpp>
#include <Gui/Utils/Keyboard.hpp>

#include <Gui/Utils/KeyMappingManager.hpp>

#include <Engine/Scene/CameraComponent.hpp>
#include <QApplication>
#include <QMessageBox>
#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>

namespace Ra {
namespace Gui {

using Core::Math::Pi;
using namespace Core::Utils;

//! [Implement KeyMappingManageable]
using FlightCameraKeyMapping = Ra::Gui::KeyMappingManageable<FlightCameraManipulator>;

#define KMA_VALUE( XX ) Gui::KeyMappingManager::KeyMappingAction Gui::FlightCameraManipulator::XX;
KeyMappingFlightManipulator
#undef KMA_VALUE

void FlightCameraManipulator::configureKeyMapping_impl() {

    FlightCameraKeyMapping::setContext(
        Gui::KeyMappingManager::getInstance()->getContext( "FlightManipulatorContext" ) );

    if ( KeyMapping::getContext().isInvalid() ) {
        LOG( logWARNING )
            << "CameraContext not defined (maybe the configuration file do not contains "
               "it). Add it for default key mapping.";
        FlightCameraKeyMapping::setContext(
            Gui::KeyMappingManager::getInstance()->addContext( "FlightManipulatorContext" ) );
    }

#define KMA_VALUE( XX )                                                                          \
    XX = Gui::KeyMappingManager::getInstance()->getAction( FlightCameraKeyMapping::getContext(), \
                                                           #XX );
    KeyMappingFlightManipulator
#undef KMA_VALUE

    auto mgr     = Gui::KeyMappingManager::getInstance();
    auto context = FlightCameraKeyMapping::getContext();

    // use wrapper to have reference in pair
    using ActionBindingPair = std::pair<std::reference_wrapper<KeyMappingManager::KeyMappingAction>,
                                        KeyMappingManager::EventBinding>;

    // don't use [] since reference don't have a default value. use at and insert instead.
    std::map<std::string, ActionBindingPair> defaultBinding;

    defaultBinding.insert(
        std::make_pair( std::string { "FLIGHTMODECAMERA_PAN" },
                        ActionBindingPair { FLIGHTMODECAMERA_PAN,
                                            mgr->createEventBindingFromStrings(
                                                "LeftButton", "ShiftModifier" ) } ) );

    defaultBinding.insert( std::make_pair(
        std::string { "FLIGHTMODECAMERA_ROTATE" },
        ActionBindingPair { FLIGHTMODECAMERA_ROTATE,
                            mgr->createEventBindingFromStrings( "LeftButton" ) } ) );

    defaultBinding.insert(
        std::make_pair( std::string { "FLIGHTMODECAMERA_ZOOM" },
                        ActionBindingPair { FLIGHTMODECAMERA_ZOOM,
                                            mgr->createEventBindingFromStrings(
                                                "LeftButton", "ControlModifier" ) } ) );

    for ( auto& [actionName, actionBinding] : defaultBinding ) {
        if ( actionBinding.first.get().isInvalid() ) {
            LOG( logWARNING ) << "FlightManipulator action " << actionName
                              << " not defined in configuration file. Adding default keymapping.";
            actionBinding.first.get() = mgr->addAction( context, actionBinding.second, actionName );
        }
    }
}
//! [Implement KeyMappingManageable]

void FlightCameraManipulator::setupKeyMappingCallbacks() {

    m_keyMappingCallbackManager.addEventCallback( FLIGHTMODECAMERA_PAN,
                                                  [=]( QEvent* event ) { panCallback( event ); } );
    m_keyMappingCallbackManager.addEventCallback(
        FLIGHTMODECAMERA_ROTATE, [=]( QEvent* event ) { rotateCallback( event ); } );

    m_keyMappingCallbackManager.addEventCallback( FLIGHTMODECAMERA_ZOOM,
                                                  [=]( QEvent* event ) { zoomCallback( event ); } );
}

void FlightCameraManipulator::panCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        auto [dx, dy]   = computeDeltaMouseMove( mouseEvent );
        handleCameraPan( dx, dy );
    }
}

void FlightCameraManipulator::rotateCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        auto [dx, dy]   = computeDeltaMouseMove( mouseEvent );
        handleCameraRotate( dx, dy );
    }
}

void FlightCameraManipulator::zoomCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        auto [dx, dy]   = computeDeltaMouseMove( mouseEvent );
        handleCameraZoom( dx, dy );
    }
    if ( event->type() == QEvent::Wheel ) {
        auto wheelEvent = reinterpret_cast<QWheelEvent*>( event );
        handleCameraZoom(
            ( wheelEvent->angleDelta().y() * 0.01_ra + wheelEvent->angleDelta().x() * 0.01_ra ) *
            m_wheelSpeedModifier );
    }
}

KeyMappingManager::Context Gui::FlightCameraManipulator::mappingContext() {
    return FlightCameraKeyMapping::getContext();
}

FlightCameraManipulator::FlightCameraManipulator() :
    CameraManipulator(), m_keyMappingCallbackManager { KeyMapping::getContext() } {
    resetCamera();
    setupKeyMappingCallbacks();
    m_cameraSensitivity = 2_ra;
}

//! [Constructor]
FlightCameraManipulator::FlightCameraManipulator( const CameraManipulator& other ) :
    CameraManipulator( other ), m_keyMappingCallbackManager { KeyMapping::getContext() } {
    m_flightSpeed = ( m_target - m_camera->getPosition() ).norm() / 10_ra;
    initializeFixedUpVector();
    setupKeyMappingCallbacks();
    m_cameraSensitivity = 2_ra;
}
//! [Constructor]

FlightCameraManipulator::~FlightCameraManipulator() = default;

void FlightCameraManipulator::updateCamera() {

    initializeFixedUpVector();

    m_target      = m_camera->getPosition() + 2_ra * m_camera->getDirection().normalized();
    m_flightSpeed = 0.2_ra;

    if ( m_light != nullptr ) {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

void FlightCameraManipulator::resetCamera() {
    m_camera->setFrame( Core::Transform::Identity() );
    m_camera->setPosition( Core::Vector3( 0, 0, 1 ) );
    initializeFixedUpVector();

    m_target      = m_camera->getPosition() + 2_ra * m_camera->getDirection().normalized();
    m_flightSpeed = 0.2_ra;

    if ( m_light != nullptr ) {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

bool FlightCameraManipulator::handleMousePressEvent( QMouseEvent* event,
                                                     const Qt::MouseButtons&,
                                                     const Qt::KeyboardModifiers&,
                                                     int key ) {
    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();
    bool handled = m_keyMappingCallbackManager.triggerEventCallback( event, key );
    return handled;
}

bool FlightCameraManipulator::handleMouseMoveEvent( QMouseEvent* event,
                                                    const Qt::MouseButtons& /*buttons*/,
                                                    const Qt::KeyboardModifiers& /*modifiers*/,
                                                    int key ) {

    bool handled = m_keyMappingCallbackManager.triggerEventCallback( event, key );

    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    if ( m_light != nullptr ) {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    return handled;
}

bool FlightCameraManipulator::handleMouseReleaseEvent( QMouseEvent* /*event*/ ) {
    return false;
}

bool FlightCameraManipulator::handleWheelEvent( QWheelEvent* event,
                                                const Qt::MouseButtons&,
                                                const Qt::KeyboardModifiers&,
                                                int key ) {
    ///\todo use action.
    bool handled = m_keyMappingCallbackManager.triggerEventCallback( event, key, true );

    if ( m_light != nullptr ) {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    return handled;
}

bool FlightCameraManipulator::handleKeyPressEvent(
    QKeyEvent* event,
    const KeyMappingManager::KeyMappingAction& action ) {
    return m_keyMappingCallbackManager.triggerEventCallback( action, event );
}

void FlightCameraManipulator::setCameraPosition( const Core::Vector3& position ) {
    if ( position == m_target ) {
        QMessageBox::warning( nullptr, "Error", "Position cannot be set to target point" );
        return;
    }
    m_camera->setPosition( position );
    m_camera->setDirection( ( m_target - position ).normalized() );

    if ( m_light != nullptr ) {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

void FlightCameraManipulator::setCameraTarget( const Core::Vector3& target ) {
    if ( m_camera->getPosition() == m_target ) {
        QMessageBox::warning( nullptr, "Error", "Target cannot be set to current camera position" );
        return;
    }

    m_target = target;
    m_camera->setDirection( ( target - m_camera->getPosition() ).normalized() );

    if ( m_light != nullptr ) { m_light->setDirection( m_camera->getDirection() ); }
}

void FlightCameraManipulator::fitScene( const Core::Aabb& aabb ) {
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

    if ( m_light != nullptr ) {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

void FlightCameraManipulator::handleCameraRotate( Scalar dx, Scalar dy ) {
    Scalar dphi   = dx * m_cameraSensitivity * m_quickCameraModifier;
    Scalar dtheta = -dy * m_cameraSensitivity * m_quickCameraModifier;

    Core::Transform R( Core::Transform::Identity() );
    if ( std::abs( dphi ) > std::abs( dtheta ) ) {
        R = Core::AngleAxis( -dphi, /*m_camera->getUpVector().normalized()*/ m_fixUpVector );
    }
    else {
        R = Core::AngleAxis( -dtheta, -m_camera->getRightVector().normalized() );
    }

    Scalar d = ( m_target - m_camera->getPosition() ).norm();

    m_camera->applyTransform( R );
    m_target = m_camera->getPosition() + d * m_camera->getDirection();
}

void FlightCameraManipulator::handleCameraPan( Scalar dx, Scalar dy ) {
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

void FlightCameraManipulator::handleCameraZoom( Scalar dx, Scalar dy ) {
    handleCameraZoom( Ra::Core::Math::sign( dy ) * ( std::abs( dx ) + std::abs( dy ) ) );
}

void FlightCameraManipulator::handleCameraZoom( Scalar z ) {
    auto y = m_flightSpeed * z * m_cameraSensitivity * m_quickCameraModifier;
    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = y * m_camera->getDirection();
    T.translate( t );
    m_camera->applyTransform( T );
    m_target += t;
}

void FlightCameraManipulator::initializeFixedUpVector() {
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
