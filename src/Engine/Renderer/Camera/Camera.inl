#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra {
namespace Engine {

/// -------------------- ///
/// FRAME
/// -------------------- ///
inline Core::Transform Camera::getFrame() const {
    return m_frame;
}

inline void Camera::setFrame( const Core::Transform& frame ) {
    m_frame = frame;
}

inline Core::Vector3 Camera::getPosition() const {
    return ( m_frame.translation() );
}

inline void Camera::setPosition( const Core::Vector3& position) {
    m_frame.translation() = position;
}

inline Core::Vector3 Camera::getDirection() const {
    return ( -m_frame.affine().block< 3, 1 >( 0, 2 ) );
}

inline void Camera::setDirection( const Core::Vector3& direction ) {
    Core::Transform T = Core::Transform::Identity();
    T.rotate( Core::Math::computeRotation(getDirection().normalized(), direction.normalized(), getUpVector()));
    applyTransform(T);
}

inline Core::Vector3 Camera::getUpVector() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 1 ) );
}

inline void Camera::setUpVector( const Core::Vector3& upVector ) {
    Core::Transform T = Core::Transform::Identity();
    T.rotate(Core::Math::computeRotation( getUpVector(), upVector.normalized(), getRightVector()));
    applyTransform(T);
}

inline Core::Vector3 Camera::getRightVector() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 0 ) );
}
/*
inline void Camera::setRightVector( const Core::Vector3& rightVector ) {
    Core::Transform T = Core::Transform::Identity();
    T.rotate(Core::Math::computeRotation( getRightVector(), rightVector.normalized(), getDirection()));
    applyTransform(T);
}*/

/// -------------------- ///
/// FIELD OF VIEW
/// -------------------- ///
inline Scalar Camera::getFOV() const {
    return m_fov;
}

inline void Camera::setFOV( const Scalar fov ) {
    m_fov = fov;
}

/// -------------------- ///
/// Z NEAR
/// -------------------- ///
inline Scalar Camera::getZNear() const {
    return m_zNear;
}

inline void Camera::setZNear( const Scalar zNear ) {
    m_zNear = zNear;
}

/// -------------------- ///
/// Z FAR
/// -------------------- ///
inline Scalar Camera::getZFar() const {
    return m_zFar;
}

inline void Camera::setZFar( const Scalar zFar ) {
    m_zFar = zFar;
}

/// -------------------- ///
/// ZOOM FACTOR
/// -------------------- ///
inline Scalar Camera::getZoomFactor() const {
    return m_zoomFactor;
}

inline void Camera::setZoomFactor( const Scalar& zoomFactor ) {
    m_zoomFactor = zoomFactor;
}

/// -------------------- ///
/// PROJECTION TYPE
/// -------------------- ///
inline Camera::ProjType Camera::getProjType() const {
    return m_projType;
}

inline void Camera::setProjType( const ProjType& projectionType ) {
    m_projType = projectionType;
}

/// -------------------- ///
/// VIEW MATRIX
/// -------------------- ///
inline Core::Matrix4 Camera::getViewMatrix() const {
    return m_viewMatrix;
}


/// -------------------- ///
/// PROJECTION MATRIX
/// -------------------- ///
inline Core::Matrix4 Camera::getProjMatrix() const {
    return m_projMatrix;
}

inline void Camera::setProjMatrix( const Core::Matrix4& projectionMatrix ) {
    m_projMatrix = projectionMatrix;
}

} // End of Engine
} // End of Ra

