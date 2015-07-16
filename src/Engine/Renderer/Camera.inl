#include <Engine/Renderer/Camera.hpp>

namespace Ra {

/// -------------------- ///
/// FRAME
/// -------------------- ///
inline Core::Transform Engine::Camera::getFrame() const {
    return m_frame;
}

inline void Engine::Camera::setFrame( const Core::Transform& frame ) {
    m_frame = frame;
}

inline Core::Vector3 Engine::Camera::getPosition() const {
    return ( m_frame.translation() );
}

inline void Engine::Camera::setPosition( const Core::Vector3& position,
                                         const ModeType mode ) {
    Core::Transform T(Core::Transform::Identity());
    T.translation() = ( position - getPosition() );
    applyTransform( T, mode );
}

inline Core::Vector3 Engine::Camera::getDirection() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 2 ) );
}

inline void Engine::Camera::setDirection( const Core::Vector3& direction ) {
    Core::Transform T = computeRotation( getDirection(), direction.normalized(), getUpVector() );
    applyTransform( T, ModeType::FREE );
}

inline Core::Vector3 Engine::Camera::getUpVector() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 1 ) );
}

inline void Engine::Camera::setUpVector( const Core::Vector3& upVector ) {
    Core::Transform T = computeRotation( getUpVector(), upVector.normalized(), getRightVector() );
    applyTransform( T, ModeType::FREE );
}

inline Core::Vector3 Engine::Camera::getRightVector() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 0 ) );
}

inline void Engine::Camera::setRightVector( const Core::Vector3& rightVector ) {
    Core::Transform T = computeRotation( getRightVector(), rightVector.normalized(), getDirection() );
    applyTransform( T, ModeType::FREE );
}

inline Core::Transform Engine::Camera::computeRotation( const Core::Vector3& v0,
                                                        const Core::Vector3& v1,
                                                        const Core::Vector3& defaultAxis ) const {
    Core::Transform T;
    T.setIdentity();
    Scalar angle = acos( v0.dot( v1 ) );
    if( angle != 0.0f ) {
        Core::Vector3 axis;
        if( v1 != -v0 ) {
            axis = ( v0.cross( v1 ) ).normalized();
        } else {
            axis = defaultAxis;
        }
        Core::AngleAxis R( angle, axis );
        T.affine().block< 3, 3 >( 0, 0 ) = R.matrix();
    }
    return T;
}




/// -------------------- ///
/// FIELD OF VIEW
/// -------------------- ///
inline Scalar Engine::Camera::getFOV() const {
    return m_fov;
}

inline void Engine::Camera::setFOV( const Scalar fov ) {
    m_fov = fov;
}




/// -------------------- ///
/// Z NEAR
/// -------------------- ///
inline Scalar Engine::Camera::getZNear() const {
    return m_zNear;
}

inline void Engine::Camera::setZNear( const Scalar zNear ) {
    m_zNear = zNear;
}




/// -------------------- ///
/// Z FAR
/// -------------------- ///
inline Scalar Engine::Camera::getZFar() const {
    return m_zFar;
}

inline void Engine::Camera::setZFar( const Scalar zFar ) {
    m_zFar = zFar;
}




/// -------------------- ///
/// FOCAL POINT
/// -------------------- ///
inline Core::Vector3 Engine::Camera::getTargetPoint() const {
    return ( getPosition() + getFocalPointDistance() * getDirection() );
}

inline void Engine::Camera::setTargetPoint( const Core::Vector3& targetPoint ) {
    Core::Vector3 direction = targetPoint - getPosition();
    setFocalPointDistance( direction.norm() );
    setDirection( direction );
}

inline Scalar Engine::Camera::getFocalPointDistance() const {
    return m_focalPoint;
}

inline void Engine::Camera::setFocalPointDistance( const Scalar focalPointDistance ) {
    m_focalPoint = focalPointDistance;
}




/// -------------------- ///
/// ZOOM FACTOR
/// -------------------- ///
inline Scalar Engine::Camera::getZoomFactor() const {
    return m_zoomFactor;
}

inline void Engine::Camera::setZoomFactor( const Scalar& zoomFactor ) {
    m_zoomFactor = zoomFactor;
}




/// -------------------- ///
/// PROJECTION TYPE
/// -------------------- ///
inline Engine::Camera::ProjType Engine::Camera::getProjType() const {
    return m_projType;
}

inline void Engine::Camera::setProjType( const ProjType& projectionType ) {
    m_projType = projectionType;
}




/// -------------------- ///
/// VIEW MATRIX
/// -------------------- ///
inline Core::Matrix4 Engine::Camera::getViewMatrix() const {
    return m_viewMatrix;
}

inline void Engine::Camera::setViewMatrix( const Core::Matrix4& viewMatrix ) {
    m_viewMatrix = viewMatrix;
}



/// -------------------- ///
/// PROJECTION MATRIX
/// -------------------- ///
inline Core::Matrix4 Engine::Camera::getProjMatrix() const {
    return m_projMatrix;
}

inline void Engine::Camera::setProjMatrix( const Core::Matrix4& projectionMatrix ) {
    m_projMatrix = projectionMatrix;
}

} // End of Ra
