#include <Gui/Viewer/TrackballCameraManipulator.hpp>

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
#include <iostream>

namespace Ra {
using Core::Math::Pi;
using namespace Ra::Core::Utils;

namespace Gui {

#define KMA_VALUE( XX ) \
    Gui::KeyMappingManager::KeyMappingAction Gui::TrackballCameraManipulator::XX;
KeyMappingCamera
#undef KMA_VALUE

    void
    Gui::TrackballCameraManipulator::configureKeyMapping_impl() {

    TrackballCameraMapping::setContext(
        Gui::KeyMappingManager::getInstance()->getContext( "CameraContext" ) );
    if ( TrackballCameraMapping::getContext().isInvalid() )
    {
        LOG( logINFO )
            << "CameraContext not defined (maybe the configuration file do not contains it)";
        LOG( logERROR ) << "CameraContext all keymapping invalide !";
        return;
    }

#define KMA_VALUE( XX )                                         \
    XX = Gui::KeyMappingManager::getInstance()->getActionIndex( \
        TrackballCameraMapping::getContext(), #XX );
    KeyMappingCamera
#undef KMA_VALUE
}

Gui::TrackballCameraManipulator::TrackballCameraManipulator() : CameraManipulator() {
    resetCamera();
}

Gui::TrackballCameraManipulator::TrackballCameraManipulator( const CameraManipulator& other ) :
    CameraManipulator( other ) {
    m_distFromCenter = ( m_target - m_camera->getPosition() ).norm();
    updatePhiTheta();
}

Gui::TrackballCameraManipulator::~TrackballCameraManipulator() = default;

void Gui::TrackballCameraManipulator::resetCamera() {
    m_camera->setFrame( Core::Transform::Identity() );
    m_camera->setPosition( Core::Vector3( 0_ra, 0_ra, 2_ra ) );
    m_target = Core::Vector3::Zero();
    m_camera->setDirection( Core::Vector3( 0_ra, 0_ra, -1_ra ) );
    m_distFromCenter = 2.0_ra;
    updatePhiTheta();

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );
}

void Gui::TrackballCameraManipulator::updateCamera() {
    // try to keep target near the previous camera's one, take it at the same distance from camera,
    // but in the new direction.
    m_distFromCenter = ( m_target - m_camera->getPosition() ).norm();
    m_target         = m_camera->getPosition() + m_distFromCenter * m_camera->getDirection();
    updatePhiTheta();

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );
}

void Gui::TrackballCameraManipulator::setTrackballRadius( Scalar rad ) {
    m_distFromCenter = rad;
    m_target         = m_camera->getPosition() + m_distFromCenter * m_camera->getDirection();
    updatePhiTheta();
}

Scalar Gui::TrackballCameraManipulator::getTrackballRadius() const {
    return m_distFromCenter;
}

void Gui::TrackballCameraManipulator::setTrackballCenter( const Core::Vector3& c ) {
    m_target         = c;
    m_distFromCenter = ( c - m_camera->getPosition() ).norm();
    updatePhiTheta();
}

const Core::Vector3& Gui::TrackballCameraManipulator::getTrackballCenter() const {
    return m_target;
}

bool Gui::TrackballCameraManipulator::handleMousePressEvent( QMouseEvent* event,
                                                             const Qt::MouseButtons& buttons,
                                                             const Qt::KeyboardModifiers& modifiers,
                                                             int key ) {
    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    m_currentAction = KeyMappingManager::getInstance()->getAction(
        TrackballCameraMapping::getContext(), buttons, modifiers, key, false );

    return m_currentAction.isValid();
}

bool Gui::TrackballCameraManipulator::handleMouseMoveEvent(
    QMouseEvent* event,
    const Qt::MouseButtons& /*buttons*/,
    const Qt::KeyboardModifiers& /* modifiers*/,
    int /*key*/ ) {

    // auto action = KeyMappingManager::getInstance()->getAction( context, buttons, modifiers, key
    // );

    Scalar dx = ( event->pos().x() - m_lastMouseX ) / m_camera->getWidth();
    Scalar dy = ( event->pos().y() - m_lastMouseY ) / m_camera->getHeight();

    if ( event->modifiers().testFlag( Qt::AltModifier ) ) { m_quickCameraModifier = 10.0_ra; }
    else
    { m_quickCameraModifier = 2.0_ra; }

    if ( m_currentAction == TRACKBALLCAMERA_ROTATE )
        handleCameraRotate( dx, dy );
    else if ( m_currentAction == TRACKBALLCAMERA_PAN )
        handleCameraPan( dx, dy );
    else if ( m_currentAction == TRACKBALLCAMERA_ZOOM )
        handleCameraZoom( dx, dy );
    else if ( m_currentAction == TRACKBALLCAMERA_MOVE_FORWARD )
        handleCameraMoveForward( dx, dy );

    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );

    return m_currentAction.isValid();
}

bool Gui::TrackballCameraManipulator::handleMouseReleaseEvent( QMouseEvent* /*event*/ ) {
    m_currentAction       = KeyMappingManager::KeyMappingAction::Invalid();
    m_quickCameraModifier = 1.0_ra;
    return true;
}

bool Gui::TrackballCameraManipulator::handleWheelEvent( QWheelEvent* event,
                                                        const Qt::MouseButtons& buttons,
                                                        const Qt::KeyboardModifiers& modifiers,
                                                        int key

) {
    auto action = KeyMappingManager::getInstance()->getAction(
        TrackballCameraMapping::getContext(), buttons, modifiers, key, true );

    if ( action == TRACKBALLCAMERA_MOVE_FORWARD )
    {
        handleCameraMoveForward(
            ( event->angleDelta().y() * 0.01_ra + event->angleDelta().x() * 0.01_ra ) *
            m_wheelSpeedModifier );
        emit cameraPositionChanged( m_camera->getPosition() );
    }
    else if ( action == TRACKBALLCAMERA_ZOOM )
    {
        handleCameraZoom(
            ( event->angleDelta().y() * 0.01_ra + event->angleDelta().x() * 0.01_ra ) *
            m_wheelSpeedModifier );
    }

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    return action.isValid();
}

bool Gui::TrackballCameraManipulator::handleKeyPressEvent(
    QKeyEvent* /*event*/,
    const KeyMappingManager::KeyMappingAction& action ) {

    using ProjType = Ra::Core::Asset::Camera::ProjType;
    if ( action == TRACKBALLCAMERA_PROJ_MODE )
    {
        m_camera->setType( m_camera->getType() == ProjType::ORTHOGRAPHIC ? ProjType::PERSPECTIVE
                                                                         : ProjType::ORTHOGRAPHIC );
    }

    return false;
}

bool Gui::TrackballCameraManipulator::handleKeyReleaseEvent( QKeyEvent* /*e*/ ) {
    return false;
}

void Gui::TrackballCameraManipulator::setCameraPosition( const Core::Vector3& position ) {
    if ( position == m_target )
    {
        QMessageBox::warning( nullptr, "Error", "Position cannot be set to target point" );
        return;
    }
    m_camera->setPosition( position );
    m_target = position + m_distFromCenter * m_camera->getDirection();
    updatePhiTheta();

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraPositionChanged( m_camera->getPosition() );
}

void Gui::TrackballCameraManipulator::setCameraTarget( const Core::Vector3& target ) {
    if ( m_camera->getPosition() == m_target )
    {
        QMessageBox::warning( nullptr, "Error", "Target cannot be set to current camera position" );
        return;
    }

    m_target = target;
    m_camera->setDirection( ( target - m_camera->getPosition() ).normalized() );
    m_distFromCenter = ( target - m_camera->getPosition() ).norm();
    updatePhiTheta();

    if ( m_light != nullptr ) { m_light->setDirection( m_camera->getDirection() ); }

    emit cameraTargetChanged( m_target );
}

void Gui::TrackballCameraManipulator::fitScene( const Core::Aabb& aabb ) {
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
    m_target         = aabb.center();
    m_distFromCenter = d;

    updatePhiTheta();

    Scalar zfar = std::max( d + ( aabb.max().z() - aabb.min().z() ) * 2_ra, m_camera->getZFar() );
    m_camera->setZFar( zfar );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_target );
}

void Gui::TrackballCameraManipulator::handleCameraRotate( Scalar dx, Scalar dy ) {
    Scalar dphi = dx * m_cameraSensitivity * m_quickCameraModifier;
    Scalar y    = -dy * m_cameraSensitivity * m_quickCameraModifier;

    Scalar phi   = std::fmod( m_phi + dphi, 2_ra * Pi );          // Keep phi between 0 and 2pi
    Scalar theta = std::fmod( m_theta + y + Pi, 2_ra * Pi ) - Pi; // Keep theta between -pi and pi

    Scalar dtheta = theta - m_theta;

    Core::Vector3 P =
        m_target + m_distFromCenter * Core::Vector3( -std::cos( phi ) * std::sin( theta ),
                                                     std::cos( theta ),
                                                     -std::sin( phi ) * std::sin( theta ) );

    Core::Vector3 t( P - m_camera->getPosition() );

    // Translate the camera given this translation
    Core::Transform T( Core::Transform::Identity() );
    T.translation() = t;

    // Rotate the camera so that it points to the center
    Core::Transform R1( Core::Transform::Identity() );
    Core::Transform R2( Core::Transform::Identity() );

    Core::Vector3 U = Core::Vector3( 0_ra, 1_ra, 0_ra );
    Core::Vector3 R = -m_camera->getRightVector().normalized();

    R1 = Core::AngleAxis( -dphi, U );
    R2 = Core::AngleAxis( -dtheta, R );

    m_camera->applyTransform( T * R1 * R2 );
    m_phi   = phi;
    m_theta = theta;
}

void Gui::TrackballCameraManipulator::handleCameraPan( Scalar dx, Scalar dy ) {
    Scalar x = dx * m_cameraSensitivity * m_quickCameraModifier * m_distFromCenter * 0.1_ra;
    Scalar y = dy * m_cameraSensitivity * m_quickCameraModifier * m_distFromCenter * 0.1_ra;
    // Move camera and trackball center, keep the distance to the center
    Core::Vector3 R = -m_camera->getRightVector();
    Core::Vector3 U = m_camera->getUpVector();

    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = x * R + y * U;
    T.translate( t );

    m_camera->applyTransform( T );
    m_target += t;
}

void Gui::TrackballCameraManipulator::handleCameraMoveForward( Scalar dx, Scalar dy ) {
    handleCameraMoveForward( Ra::Core::Math::sign( dy ) * Ra::Core::Vector2 {dx, dy}.norm() );
}

void Gui::TrackballCameraManipulator::handleCameraMoveForward( Scalar z ) {

    Scalar moveFactor = z * m_distFromCenter * m_cameraSensitivity * m_quickCameraModifier;

    Core::Transform T( Core::Transform::Identity() );
    T.translate( moveFactor * m_camera->getDirection() );

    m_camera->applyTransform( T );

    m_distFromCenter = ( m_target - m_camera->getPosition() ).norm();

    emit cameraPositionChanged( m_camera->getPosition() );
}

void Gui::TrackballCameraManipulator::handleCameraZoom( Scalar dx, Scalar dy ) {
    handleCameraZoom( Ra::Core::Math::sign( dy ) * Ra::Core::Vector2 {dx, dy}.norm() );
}

void Gui::TrackballCameraManipulator::handleCameraZoom( Scalar z ) {
    const Scalar epsIn  = 0.1_ra;
    const Scalar epsOut = 3.1_ra;
    Scalar zoom =
        std::clamp( m_camera->getZoomFactor() - z * m_cameraSensitivity * m_quickCameraModifier,
                    epsIn,
                    epsOut );
    m_camera->setZoomFactor( zoom );
}

void Gui::TrackballCameraManipulator::updatePhiTheta() {
    using Core::Math::areApproxEqual;
    const auto R = m_camera->getDirection();

    m_theta = std::acos( -R.y() );

    m_phi = ( areApproxEqual( R.z(), 0_ra ) && areApproxEqual( R.x(), 0_ra ) )
                ? std::acos( m_camera->getRightVector().dot( Ra::Core::Vector3::UnitZ() ) ) + Pi
                : std::atan2( R.z(), R.x() );

    // Keep phi between 0 and 2pi
    // Keep theta between -pi and pi
    if ( m_phi < 0_ra )
    {
        m_theta = -m_theta + Pi;
        m_phi += 2_ra * Pi;
    }

    CORE_ASSERT( std::isfinite( m_theta ) && std::isfinite( m_phi ), "Error in trackball camera" );
}

bool Gui::TrackballCameraManipulator::checkIntegrity( const std::string& mess ) const {
    Core::Vector3 c = m_camera->getPosition() + m_distFromCenter * m_camera->getDirection();
    Scalar d        = ( m_target - c ).norm();
    if ( d > 0.001_ra )
    {
        LOG( logWARNING ) << "TrackballCameraManipulator Integrity problem : " << mess;
        LOG( logWARNING ) << "\t Position " << m_camera->getPosition().transpose();
        LOG( logWARNING ) << "\t Direction " << m_camera->getDirection().transpose();
        LOG( logWARNING ) << "\t Target " << m_target.transpose();
        LOG( logWARNING ) << "\t Center " << c.transpose();
        LOG( logWARNING ) << "\t Distance " << d;
    }
    return d < 0.001_ra;
}

} // namespace Gui
} // namespace Ra
