#include <GuiBase/Viewer/TrackballCamera.hpp>

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
using Core::Math::Pi;

Gui::TrackballCamera::TrackballCamera( uint width, uint height ) :
    CameraInterface( width, height ),
    m_trackballCenter( 0, 0, 0 ),
    m_quickCameraModifier( 1.f ),
    m_wheelSpeedModifier( 0.02f ),
    m_distFromCenter( 1.0f ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    resetCamera();
}

Gui::TrackballCamera::~TrackballCamera() {}

void Gui::TrackballCamera::resetCamera() {
    m_camera->setFrame( Core::Transform::Identity() );
    m_camera->setPosition( Core::Vector3( 0, 0, 1 ) );
    m_trackballCenter = Core::Vector3::Zero();
    updatePhiTheta();

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_trackballCenter );
}

void Gui::TrackballCamera::setTrackballRadius( Scalar rad ) {
    m_distFromCenter = rad;
}

Scalar Gui::TrackballCamera::getTrackballRadius() const {
    return m_distFromCenter;
}

void Gui::TrackballCamera::setTrackballCenter( const Core::Vector3& c ) {
    m_trackballCenter = c;
    updatePhiTheta();
}

const Core::Vector3& Gui::TrackballCamera::getTrackballCenter() const {
    return m_trackballCenter;
}

bool Gui::TrackballCamera::handleMousePressEvent(
    QMouseEvent* event,
    const KeyMappingManager::KeyMappingAction& action ) {
    bool handled = false;
    m_lastMouseX = event->pos().x();
    m_lastMouseY = event->pos().y();

    if ( action == Gui::KeyMappingManager::TRACKBALLCAMERA_MANIPULATION )
    {
        m_cameraRotateMode = true;
        handled            = true;
    }
    if ( action == Gui::KeyMappingManager::TRACKBALLCAMERA_ROTATE )
    {
        m_cameraRotateMode = true;
        handled            = true;
    }
    if ( action == Gui::KeyMappingManager::TRACKBALLCAMERA_PAN )
    {
        m_cameraPanMode = true;
        handled         = true;
    }
    if ( action == Gui::KeyMappingManager::TRACKBALLCAMERA_ZOOM )
    {
        m_cameraZoomMode = true;
        handled          = true;
    }

    return handled;
}

bool Gui::TrackballCamera::handleMouseMoveEvent(
    QMouseEvent* event,
    const KeyMappingManager::KeyMappingAction& action ) {
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

    emit cameraChanged( m_camera->getPosition(), m_trackballCenter );

    return true;
}

bool Gui::TrackballCamera::handleMouseReleaseEvent(
    QMouseEvent* /*event*/,
    const KeyMappingManager::KeyMappingAction& action ) {
    m_cameraRotateMode    = false;
    m_cameraPanMode       = false;
    m_cameraZoomMode      = false;
    m_quickCameraModifier = 1.0;

    return true;
}

bool Gui::TrackballCamera::handleWheelEvent( QWheelEvent* event,
                                             const KeyMappingManager::KeyMappingAction& action ) {
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

void Gui::TrackballCamera::toggleRotateAround() {
    m_rotateAround = !m_rotateAround;
}

bool Gui::TrackballCamera::handleKeyPressEvent(
    QKeyEvent* e,
    const KeyMappingManager::KeyMappingAction& action ) {

    if ( action == Gui::KeyMappingManager::TRACKBALLCAMERA_ROTATE_AROUND )
    {
        m_rotateAround = !m_rotateAround;
        return true;
    }

    return false;
}

bool Gui::TrackballCamera::handleKeyReleaseEvent(
    QKeyEvent* /*e*/,
    const KeyMappingManager::KeyMappingAction& action ) {
    return false;
}

void Gui::TrackballCamera::setCamera( Engine::Camera* camera ) {
    if ( !camera ) return;
    camera->resize( m_camera->getWidth(), m_camera->getHeight() );
    m_camera          = camera;
    m_trackballCenter = m_camera->getPosition() + 2 * m_camera->getDirection().normalized();
    m_distFromCenter  = 2.0;
    updatePhiTheta();
    m_camera->show( false );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

void Gui::TrackballCamera::setCameraPosition( const Core::Vector3& position ) {
    if ( position == m_trackballCenter )
    {
        QMessageBox::warning( nullptr, "Error", "Position cannot be set to target point" );
        return;
    }
    m_camera->setPosition( position );
    m_camera->setDirection( m_trackballCenter - position );

    updatePhiTheta();

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraPositionChanged( m_camera->getPosition() );
}

void Gui::TrackballCamera::setCameraTarget( const Core::Vector3& target ) {
    if ( m_camera->getPosition() == m_trackballCenter )
    {
        QMessageBox::warning( nullptr, "Error", "Target cannot be set to current camera position" );
        return;
    }

    m_trackballCenter = target;
    m_camera->setDirection( target - m_camera->getPosition() );
    updatePhiTheta();

    if ( m_light != nullptr ) { m_light->setDirection( m_camera->getDirection() ); }

    emit cameraTargetChanged( m_trackballCenter );
}

void Gui::TrackballCamera::fitScene( const Core::Aabb& aabb ) {
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
    m_trackballCenter = aabb.center();

    updatePhiTheta();

    m_distFromCenter = d;

    Scalar zfar = std::max( d + ( aabb.max().z() - aabb.min().z() ) * 2_ra, m_camera->getZFar() );
    m_camera->setZFar( zfar );

    if ( m_light != nullptr )
    {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }

    emit cameraChanged( m_camera->getPosition(), m_trackballCenter );
}

void Gui::TrackballCamera::handleCameraRotate( Scalar dx, Scalar dy ) {
    Scalar x = dx * m_cameraSensitivity * m_quickCameraModifier;
    Scalar y = -dy * m_cameraSensitivity * m_quickCameraModifier;

    Scalar phi   = m_phi + x; // Keep phi between -pi and pi
    Scalar theta = std::fmod( m_theta + y + Pi, 2_ra * Pi ) - Pi;

    Scalar dphi   = phi - m_phi;
    Scalar dtheta = theta - m_theta;

    const Core::Vector3 C  = m_trackballCenter;
    const Core::Vector3 P0 = m_camera->getPosition();
    const Scalar r         = ( C - P0 ).norm();
    // Compute new camera position, on the sphere of radius r centered on C
    Scalar px = C.x() + r * std::cos( phi ) * std::sin( theta );
    Scalar py = C.y() + r * std::cos( theta );
    Scalar pz = C.z() + r * std::sin( phi ) * std::sin( theta );

    // Compute the translation from old pos to new pos
    Core::Vector3 P( px, py, pz );
    Core::Vector3 t( P - P0 );

    // Translate the camera given this translation
    Core::Transform T( Core::Transform::Identity() );
    T.translation() = t;

    // Rotate the camera so that it points to the center
    Core::Transform R1( Core::Transform::Identity() );
    Core::Transform R2( Core::Transform::Identity() );

    Core::Vector3 U = Core::Vector3( 0, 1, 0 );
    Core::Vector3 R = -m_camera->getRightVector().normalized();

    R1 = Core::AngleAxis( -dphi, U );
    R2 = Core::AngleAxis( -dtheta, R );

    m_camera->applyTransform( T * R1 * R2 );

    m_phi   = phi;
    m_theta = theta;
}

void Gui::TrackballCamera::handleCameraPan( Scalar dx, Scalar dy ) {
    Scalar x = dx * m_cameraSensitivity * m_quickCameraModifier * m_distFromCenter * 0.1_ra;
    Scalar y = dy * m_cameraSensitivity * m_quickCameraModifier * m_distFromCenter * 0.1_ra;
    // Move camera and trackball center, keep the distance to the center
    Core::Vector3 R = -m_camera->getRightVector();
    Core::Vector3 U = m_camera->getUpVector();

    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = x * R + y * U;
    T.translate( t );

    m_camera->applyTransform( T );
    m_trackballCenter += t;
}

void Gui::TrackballCamera::handleCameraZoom( Scalar dx, Scalar dy ) {
    handleCameraZoom( Ra::Core::Math::sign( dx ) * ( std::abs( dx ) + std::abs( dy ) ) );
}

void Gui::TrackballCamera::handleCameraZoom( Scalar z ) {
    // tested this way of zooming, not convinced it's better
#if 0
    Scalar zoom = m_camera->getZoomFactor() - z * m_cameraSensitivity * m_quickCameraModifier;
    Scalar epsIn = 0.001;
    Scalar epsOut = 3.1;
    m_camera->setZoomFactor( std::clamp( zoom, epsIn, epsOut ) );
#else
    Scalar y        = m_distFromCenter * z * m_cameraSensitivity * m_quickCameraModifier;
    Core::Vector3 F = m_camera->getDirection();

    Scalar dist = ( m_trackballCenter - m_camera->getPosition() ).norm();

    if ( dist < ( m_camera->getZNear() + y ) ) { y = dist - m_camera->getZNear(); }

    Core::Transform T( Core::Transform::Identity() );
    Core::Vector3 t = y * F;
    T.translate( t );

    m_camera->applyTransform( T );

    // m_trackballCenter = m_camera->getPosition() + m_camera->getDirection().normalized();

    emit cameraPositionChanged( m_camera->getPosition() );

    // m_distFromCenter = ( m_trackballCenter - m_camera->getPosition() ).norm();
#endif
}

void Gui::TrackballCamera::updatePhiTheta() {
    using Core::Math::areApproxEqual;
    const Core::Vector3& P = m_camera->getPosition();
    const Core::Vector3& C = m_trackballCenter;
    const Core::Vector3& R = P - C;
    const Scalar r         = R.norm();

    m_theta = std::acos( R.y() / r );
    m_phi   = ( areApproxEqual( R.z(), 0_ra ) && areApproxEqual( R.x(), 0_ra ) )
                ? 0_ra
                : std::atan2( R.z(), R.x() );
    CORE_ASSERT( std::isfinite( m_theta ) && std::isfinite( m_phi ), "Error in trackball camera" );
}
} // namespace Ra
