#pragma once
#include <Core/Asset/Camera.hpp>
#include <Engine/Scene/CameraComponent.hpp>
#include <Engine/Scene/Entity.hpp>

namespace Ra {
namespace Engine {
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
    m_camera->setViewport( width, height );
}

inline Scalar CameraComponent::getAspect() const {
    return m_camera->getAspect();
}

inline Ra::Core::Asset::Camera::ProjType CameraComponent::getType() const {
    return m_camera->getType();
}

inline void CameraComponent::setType( const Ra::Core::Asset::Camera::ProjType& projectionType ) {
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
