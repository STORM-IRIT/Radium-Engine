#include <GuiBase/Viewer/TrackballCameraManipulator.hpp>

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

#define KMA_VALUE( XX ) \
    Gui::KeyMappingManager::KeyMappingAction Gui::TrackballCameraManipulator::XX;
KeyMappingCamera
#undef KMA_VALUE

    void
    Gui::TrackballCameraManipulator::configureKeyMapping_impl() {

    m_keyMappingContext = Gui::KeyMappingManager::getInstance()->getContext( "CameraContext" );
    if ( m_keyMappingContext.isInvalid() )
    {
        LOG( Ra::Core::Utils::logINFO )
            << "CameraContext not defined (maybe the configuration file do not contains it)";
        LOG( Ra::Core::Utils::logERROR ) << "CameraContext all keymapping invalide !";
        return;
    }

#define KMA_VALUE( XX ) \
    XX = Gui::KeyMappingManager::getInstance()->getActionIndex( m_keyMappingContext, #XX );
    KeyMappingCamera
#undef KMA_VALUE
}

Gui::TrackballCameraManipulator::TrackballCameraManipulator( uint width, uint height ) :
    CameraManipulator( width, height ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    resetCamera();
}

Gui::TrackballCameraManipulator::TrackballCameraManipulator( const CameraManipulator& other ) :
    CameraManipulator( other ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
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
    bool handled = false;
    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    auto action = KeyMappingManager::getInstance()->getAction(
        m_keyMappingContext, buttons, modifiers, key, false );

    if ( action == TRACKBALLCAMERA_ROTATE )
    {
        m_cameraRotateMode = true;
        handled            = true;
    }
    if ( action == TRACKBALLCAMERA_PAN )
    {
        m_cameraPanMode = true;
        handled         = true;
    }
    if ( action == TRACKBALLCAMERA_ZOOM )
    {
        m_cameraZoomMode = true;
        handled          = true;
    }

    return handled;
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

bool Gui::TrackballCameraManipulator::handleMouseReleaseEvent( QMouseEvent* /*event*/ ) {
    m_cameraRotateMode    = false;
    m_cameraPanMode       = false;
    m_cameraZoomMode      = false;
    m_quickCameraModifier = 1.0_ra;

    return true;
}

bool Gui::TrackballCameraManipulator::handleWheelEvent( QWheelEvent* event ) {

    handleCameraZoom( ( event->angleDelta().y() * 0.01_ra + event->angleDelta().x() * 0.01_ra ) *
                      m_wheelSpeedModifier );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraPositionChanged( m_camera->getPosition() );

    return true;
}

void Gui::TrackballCameraManipulator::toggleRotateAround() {
    m_rotateAround = !m_rotateAround;
}

bool Gui::TrackballCameraManipulator::handleKeyPressEvent(
    QKeyEvent* /*event*/,
    const KeyMappingManager::KeyMappingAction& action ) {

    if ( action == TRACKBALLCAMERA_ROTATE_AROUND )
    {
        m_rotateAround = !m_rotateAround;
        return true;
    }

    return false;
}

bool Gui::TrackballCameraManipulator::handleKeyReleaseEvent( QKeyEvent* /*e*/ ) {
    return false;
}

void Gui::TrackballCameraManipulator::setCamera( Engine::Camera* camera ) {
    if ( !camera ) return;
    camera->resize( m_camera->getWidth(), m_camera->getHeight() );
    m_camera         = camera;
    m_target         = m_camera->getPosition() + 2 * m_camera->getDirection().normalized();
    m_distFromCenter = 2.0_ra;
    updatePhiTheta();
    m_camera->show( false );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

void Gui::TrackballCameraManipulator::setCameraPosition( const Core::Vector3& position ) {
    if ( position == m_target )
    {
        QMessageBox::warning( nullptr, "Error", "Position cannot be set to target point" );
        return;
    }
    m_camera->setPosition( position );
    /*
    m_camera->setDirection( (m_target - position).normalized() );
    m_distFromCenter = (m_target - position).norm();
    updatePhiTheta();
    */
    m_target = position + m_distFromCenter * m_camera->getDirection().normalized();

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

void Gui::TrackballCameraManipulator::handleCameraZoom( Scalar dx, Scalar dy ) {
    handleCameraZoom( Ra::Core::Math::sign( dx ) * ( std::abs( dx ) + std::abs( dy ) ) );
}

void Gui::TrackballCameraManipulator::handleCameraZoom( Scalar z ) {
    // tested this way of zooming, not convinced it's better
#if 0
    Scalar zoom = m_camera->getZoomFactor() - z * m_cameraSensitivity * m_quickCameraModifier;
    Scalar epsIn = 0.001;
    Scalar epsOut = 3.1;
    m_camera->setZoomFactor( std::clamp( zoom, epsIn, epsOut ) );
#else
    Scalar y    = m_distFromCenter * z * m_cameraSensitivity * m_quickCameraModifier;
    Scalar dist = ( m_target - m_camera->getPosition() ).norm();
    if ( dist < ( m_camera->getZNear() + y ) ) { y = dist - m_camera->getZNear(); }

    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = y * m_camera->getDirection();
    T.translate( t );

    m_camera->applyTransform( T );
    m_distFromCenter = ( m_target - m_camera->getPosition() ).norm();

    emit cameraPositionChanged( m_camera->getPosition() );
#endif
}

void Gui::TrackballCameraManipulator::updatePhiTheta() {
    using Core::Math::areApproxEqual;
    const auto R = m_camera->getDirection().normalized();

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
        LOG( Ra::Core::Utils::logWARNING )
            << "TrackballCameraManipulator Integrity problem : " << mess;
        LOG( Ra::Core::Utils::logWARNING ) << "\t Position " << m_camera->getPosition().transpose();
        LOG( Ra::Core::Utils::logWARNING )
            << "\t Direction " << m_camera->getDirection().transpose();
        LOG( Ra::Core::Utils::logWARNING ) << "\t Target " << m_target.transpose();
        LOG( Ra::Core::Utils::logWARNING ) << "\t Center " << c.transpose();
        LOG( Ra::Core::Utils::logWARNING ) << "\t Distance " << d;
    }
    return d < 0.001_ra;
}

} // namespace Gui
} // namespace Ra
