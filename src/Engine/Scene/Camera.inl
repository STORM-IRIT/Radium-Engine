#pragma once
#include <Engine/Scene/Camera.hpp>
#include <Engine/Scene/Entity.hpp>

namespace Ra {
namespace Engine {

namespace Data {

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
    return ( -m_frame.affine().block<3, 1>( 0, 2 ) ).normalized();
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

inline Core::Vector2 Camera::project( const Core::Vector3& p ) const {
    Core::Vector4 point = Core::Vector4::Ones();
    point.head<3>()     = p;
    auto vpPoint        = getProjMatrix() * getViewMatrix() * point;

    return Core::Vector2( getWidth() * Scalar( 0.5 ) * ( vpPoint.x() + Scalar( 1 ) ),
                          getHeight() * Scalar( 0.5 ) * ( vpPoint.y() + Scalar( -1 ) ) );
}

inline Core::Vector3 Camera::unProject( const Core::Vector2& pix ) const {
    const Scalar localX = ( Scalar( 2 ) * pix.x() ) / getWidth() - Scalar( 1 );
    // Y is "inverted" (goes downwards)
    const Scalar localY = -( Scalar( 2 ) * pix.y() ) / getHeight() + Scalar( 1 );

    // Multiply the point in screen space by the inverted projection matrix
    // and then by the inverted view matrix ( = m_frame) to get it in world space.
    // NB : localPoint needs to be a vec4 to be multiplied by the proj matrix.
    const Core::Vector4 localPoint( localX, localY, -getZNear(), Scalar( 1 ) );
    const Core::Vector4 unproj = getProjMatrix().inverse() * localPoint;
    return getFrame() * unproj.head<3>();
}
} // namespace Data

namespace Scene {

inline Core::Transform CameraComponent::getFrame() const {
    return m_camera->getFrame();
}

inline void CameraComponent::setFrame( const Core::Transform& frame ) {
    m_camera->setFrame( frame );
}

inline Core::Vector3 CameraComponent::getPosition() const {
    return m_camera->getPosition();
}

inline void CameraComponent::setPosition( const Core::Vector3& position ) {
    m_camera->setPosition( position );
}

inline Core::Vector3 CameraComponent::getDirection() const {
    return m_camera->getDirection();
}

inline void CameraComponent::setDirection( const Core::Vector3& direction ) {
    m_camera->setDirection( direction );
}

inline Core::Vector3 CameraComponent::getUpVector() const {
    return m_camera->getUpVector();
}

inline void CameraComponent::setUpVector( const Core::Vector3& upVector ) {
    m_camera->setUpVector( upVector );
}

inline Core::Vector3 CameraComponent::getRightVector() const {
    return m_camera->getRightVector();
}

inline Scalar CameraComponent::getFOV() const {
    return m_camera->getFOV();
}

inline void CameraComponent::setFOV( Scalar fov ) {
    m_camera->setFOV( fov );
}

inline Scalar CameraComponent::getZNear() const {
    return m_camera->getZNear();
}

inline void CameraComponent::setZNear( Scalar zNear ) {
    m_camera->setZNear( zNear );
}

inline Scalar CameraComponent::getZFar() const {
    return m_camera->getZFar();
}

inline void CameraComponent::setZFar( Scalar zFar ) {
    m_camera->setZFar( zFar );
}

inline Scalar CameraComponent::getZoomFactor() const {
    return m_camera->getZoomFactor();
}

inline void CameraComponent::setZoomFactor( const Scalar& zoomFactor ) {
    m_camera->setZoomFactor( zoomFactor );
}

inline Scalar CameraComponent::getWidth() const {
    return m_camera->getWidth();
}

inline Scalar CameraComponent::getHeight() const {
    return m_camera->getHeight();
}

inline void CameraComponent::resize( Scalar width, Scalar height ) {
    m_camera->resize( width, height );
}

inline Scalar CameraComponent::getAspect() const {
    return m_camera->getAspect();
}

inline Ra::Engine::Data::Camera::ProjType CameraComponent::getType() const {
    return m_camera->getType();
}

inline void CameraComponent::setType( const Ra::Engine::Data::Camera::ProjType& projectionType ) {
    m_camera->setType( projectionType );
}

inline Core::Matrix4 CameraComponent::getViewMatrix() const {
    return m_camera->getViewMatrix();
}

inline Core::Matrix4 CameraComponent::getProjMatrix() const {
    return m_camera->getProjMatrix();
}

inline Core::Vector2 CameraComponent::project( const Core::Vector3& p ) const {
    return m_camera->project( p );
}

inline Core::Vector3 CameraComponent::unProject( const Core::Vector2& pix ) const {
    return m_camera->unProject( pix );
}
} // namespace Scene
} // namespace Engine
} // namespace Ra
