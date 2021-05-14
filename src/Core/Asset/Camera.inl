#pragma once

#include <Core/Asset/Camera.hpp>

namespace Ra {
namespace Core {
namespace Asset {

inline Core::Transform Camera::getFrame() const {
    return m_frame;
}

inline void Camera::setFrame( const Core::Transform& frame ) {
    m_frame = frame;
}

inline Core::Vector3 Camera::getPosition() const {
    return ( m_frame.translation() );
}

inline void Camera::setPosition( const Core::Vector3& position ) {
    Core::Transform T = Core::Transform::Identity();
    T.translation()   = position - m_frame.translation();
    applyTransform( T );
}

inline Core::Vector3 Camera::getDirection() const {
    return ( -m_frame.linear().block<3, 1>( 0, 2 ) ).normalized();
}

inline Core::Vector3 Camera::getUpVector() const {
    return ( m_frame.affine().block<3, 1>( 0, 1 ) );
}

inline void Camera::setUpVector( const Core::Vector3& upVector ) {
    Core::Transform T = Core::Transform::Identity();
    T.rotate( Core::Quaternion::FromTwoVectors( getUpVector(), upVector ) );
    applyTransform( T );
}

inline Core::Vector3 Camera::getRightVector() const {
    return ( m_frame.affine().block<3, 1>( 0, 0 ) );
}

inline Scalar Camera::getFOV() const {
    return m_fov;
}

inline void Camera::setFOV( Scalar fov ) {
    m_fov = fov;
    updateProjMatrix();
}

inline Scalar Camera::getZNear() const {
    return m_zNear;
}

inline void Camera::setZNear( Scalar zNear ) {
    m_zNear = zNear;
    updateProjMatrix();
}

inline Scalar Camera::getZFar() const {
    return m_zFar;
}

inline void Camera::setZFar( Scalar zFar ) {
    m_zFar = zFar;
    updateProjMatrix();
}

inline Scalar Camera::getZoomFactor() const {
    return m_zoomFactor;
}

inline void Camera::setZoomFactor( const Scalar& zoomFactor ) {
    m_zoomFactor = zoomFactor;
    updateProjMatrix();
}

inline Scalar Camera::getWidth() const {
    return m_width;
}

inline Scalar Camera::getHeight() const {
    return m_height;
}

inline Scalar Camera::getAspect() const {
    return m_aspect;
}

inline Camera::ProjType Camera::getType() const {
    return m_projType;
}

inline void Camera::setType( const ProjType& projectionType ) {
    m_projType = projectionType;
}

inline Core::Matrix4 Camera::getViewMatrix() const {
    return getFrame().inverse().matrix();
}

inline Core::Matrix4 Camera::getProjMatrix() const {
    return m_projMatrix;
}

inline Scalar Camera::getMinZNear() const {
    return m_minZNear;
}

inline Scalar Camera::getMinZRange() const {
    return m_minZRange;
}

inline void Camera::setProjMatrix( Core::Matrix4 projMatrix ) {
    m_projMatrix = projMatrix;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
