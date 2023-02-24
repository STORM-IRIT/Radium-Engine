#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Rendering/Renderer.hpp>
#include <Engine/Scene/Light.hpp>
#include <Engine/Scene/SystemDisplay.hpp>
#include <Gui/Utils/KeyMappingManager.hpp>
#include <Gui/Viewer/RotateAroundCameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>

namespace Ra {
namespace Gui {

using namespace Ra::Core::Utils;

#define KMA_VALUE( XX ) KeyMappingManager::KeyMappingAction RotateAroundCameraManipulator::XX;
KeyMappingRotateAroundCamera
#undef KMA_VALUE

void RotateAroundCameraManipulator::configureKeyMapping_impl() {
    auto keyMappingManager = Gui::KeyMappingManager::getInstance();

    KeyMapping::setContext( KeyMappingManager::getInstance()->getContext( "CameraContext" ) );
    if ( KeyMapping::getContext().isInvalid() ) {
        LOG( logINFO )
            << "CameraContext not defined (maybe the configuration file do not contains it)";
        LOG( logERROR ) << "CameraContext all keymapping invalid !";
        return;
    }

#define KMA_VALUE( XX ) XX = keyMappingManager->getAction( KeyMapping::getContext(), #XX );
    KeyMappingRotateAroundCamera
#undef KMA_VALUE
}

void RotateAroundCameraManipulator::setupKeyMappingCallbacks() {

    m_keyMappingCallbackManager.addEventCallback(
        TRACKBALLCAMERA_ROTATE, [=]( QEvent* event ) { rotateCallback( event ); } );
    m_keyMappingCallbackManager.addEventCallback( TRACKBALLCAMERA_PAN,
                                                  [=]( QEvent* event ) { panCallback( event ); } );
    m_keyMappingCallbackManager.addEventCallback( TRACKBALLCAMERA_ZOOM,
                                                  [=]( QEvent* event ) { zoomCallback( event ); } );
    m_keyMappingCallbackManager.addEventCallback(
        TRACKBALLCAMERA_MOVE_FORWARD, [=]( QEvent* event ) { moveForwardCallback( event ); } );
    m_keyMappingCallbackManager.addEventCallback(
        ROTATEAROUND_ALIGN_WITH_CLOSEST_AXIS,
        [=]( QEvent* event ) { alignWithClosestAxisCallback( event ); } );
    m_keyMappingCallbackManager.addEventCallback(
        ROTATEAROUND_SET_PIVOT, [=]( QEvent* event ) { setPivotCallback( event ); } );
}

void RotateAroundCameraManipulator::alignWithClosestAxisCallback( QEvent* event ) {
    if ( event->type() == QEvent::KeyPress ) { alignOnClosestAxis(); }
}

void RotateAroundCameraManipulator::moveForwardCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        auto [dx, dy]   = computeDeltaMouseMove( mouseEvent );
        handleCameraMoveForward( dx, dy );
    }
}
void RotateAroundCameraManipulator::panCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        auto [dx, dy]   = computeDeltaMouseMove( mouseEvent );
        handleCameraPan( dx, dy );
    }
}
void RotateAroundCameraManipulator::rotateCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        handleCameraRotate( mouseEvent->pos().x(), mouseEvent->pos().y() );
    }
}
void RotateAroundCameraManipulator::setPivotCallback( QEvent* event ) {
    if ( event->type() == QEvent::KeyPress ) { setPivotFromPixel( m_lastMouseX, m_lastMouseY ); }
}
void RotateAroundCameraManipulator::zoomCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        auto [dx, dy]   = computeDeltaMouseMove( mouseEvent );
        handleCameraZoom( dx, dy );
    }
}

void rotateAroundPoint( Ra::Core::Asset::Camera* cam,
                        Ra::Core::Vector3& target,
                        Ra::Core::Quaternion& rotation,
                        const Ra::Core::Vector3& point ) {
    Ra::Core::Vector3 t = cam->getPosition();
    Scalar l            = ( target - t ).norm();
    Ra::Core::Transform inverseCamRotateAround;
    Ra::Core::AngleAxis aa0 { rotation };
    Ra::Core::AngleAxis aa1 { aa0.angle(), ( cam->getFrame().linear() * aa0.axis() ).normalized() };
    inverseCamRotateAround.setIdentity();
    inverseCamRotateAround.rotate( aa1 );
    cam->applyTransform( inverseCamRotateAround );

    Ra::Core::Vector3 trans = point + inverseCamRotateAround * ( t - point );
    cam->setPosition( trans );
    target = cam->getPosition() + l * cam->getDirection();
}

RotateAroundCameraManipulator::RotateAroundCameraManipulator( Ra::Gui::Viewer* viewer ) :
    TrackballCameraManipulator(),
    m_keyMappingCallbackManager { KeyMapping::getContext() },
    m_viewer( viewer ) {
    setupKeyMappingCallbacks();
    m_cameraSensitivity = 0.5_ra;
}

RotateAroundCameraManipulator::RotateAroundCameraManipulator( const CameraManipulator& cm,
                                                              Ra::Gui::Viewer* viewer ) :
    TrackballCameraManipulator( cm ),
    m_keyMappingCallbackManager { KeyMapping::getContext() },
    m_viewer( viewer ) {
    setupKeyMappingCallbacks();
    m_cameraSensitivity = 0.5_ra;
}

bool RotateAroundCameraManipulator::handleMouseMoveEvent(
    QMouseEvent* event,
    const Qt::MouseButtons& /*buttons*/,
    const Qt::KeyboardModifiers& /* modifiers*/,
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

bool RotateAroundCameraManipulator::handleKeyPressEvent(
    QKeyEvent* event,
    const Ra::Gui::KeyMappingManager::KeyMappingAction& action ) {
    return m_keyMappingCallbackManager.triggerEventCallback( action, event );
}

void RotateAroundCameraManipulator::setPivot( Ra::Core::Vector3 pivot ) {
    m_pivot = pivot;
}

void RotateAroundCameraManipulator::setPivotFromPixel( Scalar x, Scalar y ) {
    using Ra::Core::Vector3;
    using Ra::Core::Utils::Color;

    auto dc = m_viewer->toDevice( { x, y } );

    m_viewer->makeCurrent();
    Scalar z = m_viewer->getDepthUnderMouse();
    m_viewer->doneCurrent();

    auto pivotPoint = m_camera->unProjectFromScreen( Vector3( dc[0], dc[1], z ) );

    setPivot( pivotPoint );

    // draw a 5 logical pixel sphere, shift lc by 5 pixels
    auto dcs          = m_viewer->toDevice( { x + 5, y } );
    auto pivotShifted = m_camera->unProjectFromScreen( Vector3( dcs[0], dcs[1], z ) );
    auto radius       = ( pivotPoint - pivotShifted ).norm();

    auto id = RA_DISPLAY_SPHERE( pivotPoint, radius, Color::Yellow() );
    auto ro = Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( id );
    ro->setLifetime( 100 );
}

void RotateAroundCameraManipulator::alignOnClosestAxis() {
    Ra::Core::Vector3 x( 1_ra, 0_ra, 0_ra );
    Ra::Core::Vector3 y( 0_ra, 1_ra, 0_ra );
    Ra::Core::Vector3 z( 0_ra, 0_ra, 1_ra );
    Ra::Core::Vector3 oldDirection, newDirection = m_camera->getDirection();
    Ra::Core::Vector3 newUpVector = m_camera->getUpVector();
    // getFrame().inverse() is a transform we can apply on Vector3
    Ra::Core::Vector3 pivotInCamSpace = m_camera->getFrame().inverse() * m_pivot;

    auto updateMaxAndAxis =
        []( Ra::Core::Vector3 ref, Ra::Core::Vector3 axis, Ra::Core::Vector3& out, Scalar& max ) {
            Scalar d = ref.dot( axis );
            if ( d > max ) {
                max = d;
                out = axis;
            }
        };

    Scalar max            = 0_ra;
    Ra::Core::Vector3 ref = m_camera->getDirection();
    for ( auto axis : std::vector<Ra::Core::Vector3> { -x, x, -y, y, -z, z } ) {
        updateMaxAndAxis( ref, axis, newDirection, max );
    }
    m_camera->setDirection( newDirection );

    max = 0_ra;
    ref = m_camera->getUpVector();
    for ( auto axis : std::vector<Ra::Core::Vector3> { -x, x, -y, y, -z, z } ) {
        updateMaxAndAxis( ref, axis, newUpVector, max );
    }
    m_camera->setUpVector( newUpVector );

    Ra::Core::Vector3 newPivot = m_camera->getFrame() * pivotInCamSpace;
    Ra::Core::Vector3 trans    = m_pivot - newPivot;
    m_camera->setPosition( m_camera->getPosition() + trans );

    if ( m_light != nullptr ) {
        m_light->setPosition( m_camera->getPosition() );
        m_light->setDirection( m_camera->getDirection() );
    }
}

KeyMappingManager::Context RotateAroundCameraManipulator::mappingContext() {
    return KeyMapping::getContext();
}

void RotateAroundCameraManipulator::fitScene( const Core::Aabb& aabb ) {
    TrackballCameraManipulator::fitScene( aabb );
    setPivot( aabb.center() );
}

void RotateAroundCameraManipulator::handleCameraRotate( Scalar x, Scalar y ) {
    Ra::Core::Vector3 trans  = m_camera->projectToScreen( m_pivot );
    Ra::Core::Quaternion rot = deformedBallQuaternion( x, y, trans[0], trans[1] );
    Ra::Core::Vector3 pivot  = m_pivot;
    rotateAroundPoint( m_camera, m_target, rot, pivot );
}

Scalar RotateAroundCameraManipulator::projectOnBall( Scalar x, Scalar y ) {
    const Scalar size       = 1.0;
    const Scalar size2      = size * size;
    const Scalar size_limit = size2 * 0.5;

    const Scalar d = x * x + y * y;
    return d < size_limit ? std::sqrt( size2 - d ) : size_limit / std::sqrt( d );
}

Ra::Core::Quaternion
RotateAroundCameraManipulator::deformedBallQuaternion( Scalar x, Scalar y, Scalar cx, Scalar cy ) {
    // Points on the deformed ball
    Scalar px = m_cameraSensitivity * ( m_lastMouseX - cx ) / m_camera->getWidth();
    Scalar py = m_cameraSensitivity * ( cy - m_lastMouseY ) / m_camera->getHeight();
    Scalar dx = m_cameraSensitivity * ( x - cx ) / m_camera->getWidth();
    Scalar dy = m_cameraSensitivity * ( cy - y ) / m_camera->getHeight();

    const Ra::Core::Vector3 p1( px, py, projectOnBall( px, py ) );
    const Ra::Core::Vector3 p2( dx, dy, projectOnBall( dx, dy ) );
    // Approximation of rotation angle
    // Should be divided by the projectOnBall size, but it is 1.0
    Ra::Core::Vector3 axis = p2.cross( p1 );

    if ( axis.norm() > 10_ra * Ra::Core::Math::machineEps ) {
        const Scalar angle =
            5.0_ra *
            std::asin( std::sqrt( axis.squaredNorm() / p1.squaredNorm() / p2.squaredNorm() ) );
        return Ra::Core::Quaternion( Ra::Core::AngleAxis( angle, axis.normalized() ) );
    }
    return Ra::Core::Quaternion {
        Ra::Core::AngleAxis( 0_ra, Ra::Core::Vector3( 0_ra, 0_ra, 1_ra ) ) };
}

void RotateAroundCameraManipulator::handleCameraForward( Scalar z ) {
    Ra::Core::Vector3 trans = m_camera->getDirection() * z;
    Ra::Core::Transform transfo( Ra::Core::Transform::Identity() );
    transfo.translate( trans );

    m_camera->applyTransform( transfo );
}

void RotateAroundCameraManipulator::handleCameraPan( Scalar dx, Scalar dy ) {
    Scalar x = dx * m_cameraSensitivity * m_quickCameraModifier * m_distFromCenter * 0.1_ra;
    Scalar y = dy * m_cameraSensitivity * m_quickCameraModifier * m_distFromCenter * 0.1_ra;
    // Move camera and trackball center, keep the distance to the center
    Ra::Core::Vector3 R = -m_camera->getRightVector();
    Ra::Core::Vector3 U = m_camera->getUpVector();

    Ra::Core::Transform T( Ra::Core::Transform::Identity() );
    Ra::Core::Vector3 t = x * R + y * U;
    T.translate( t );
    m_target += t;

    m_camera->applyTransform( T );
}

} // namespace Gui
} // namespace Ra
