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

inline void Camera::setPosition( const Core::Vector3& position,
                                         const ModeType mode ) {
    Core::Transform T(Core::Transform::Identity());
    T.translation() = ( position - getPosition() );
    applyTransform( T, mode );
}

inline Core::Vector3 Camera::getDirection() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 2 ) );
}

inline void Camera::setDirection( const Core::Vector3& direction ) {
    Core::Transform T = computeRotation( getDirection(), direction.normalized(), getUpVector() );
    applyTransform( T, ModeType::FREE );
}

inline Core::Vector3 Camera::getUpVector() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 1 ) );
}

inline void Camera::setUpVector( const Core::Vector3& upVector ) {
    Core::Transform T = computeRotation( getUpVector(), upVector.normalized(), getRightVector() );
    applyTransform( T, ModeType::FREE );
}

inline Core::Vector3 Camera::getRightVector() const {
    return ( m_frame.affine().block< 3, 1 >( 0, 0 ) );
}

inline void Camera::setRightVector( const Core::Vector3& rightVector ) {
    Core::Transform T = computeRotation( getRightVector(), rightVector.normalized(), getDirection() );
    applyTransform( T, ModeType::FREE );
}

inline Core::Transform Camera::computeRotation( const Core::Vector3& v0,
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
/// FOCAL POINT
/// -------------------- ///
inline Core::Vector3 Camera::getTargetPoint() const {
    return ( getPosition() + getFocalPointDistance() * getDirection() );
}

inline void Camera::setTargetPoint( const Core::Vector3& targetPoint ) {
    Core::Vector3 direction = targetPoint - getPosition();
    Scalar        norm      = direction.norm();
    setFocalPointDistance( norm );
    if( norm != 0.0 ) setDirection( direction );
}

inline Scalar Camera::getFocalPointDistance() const {
    return m_focalPoint;
}

inline void Camera::setFocalPointDistance( const Scalar focalPointDistance ) {
    if( focalPointDistance < 0.0f ) {
        m_focalPoint = -focalPointDistance;
        setDirection( -getDirection() );
    } else {
        m_focalPoint = focalPointDistance;
    }
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

inline void Camera::setViewMatrix( const Core::Matrix4& viewMatrix ) {
    m_viewMatrix = viewMatrix;
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

/// ------------------------ ///
/// HIGH LEVEL MANIPULATIONS
/// ------------------------ ///

inline void Camera::strafeLeft(Scalar amount)
{
    Core::Vector3 pos = getPosition();
    Core::Vector3 right = getRightVector();

    setPosition(pos + amount * right);
}

inline void Camera::strafeRight(Scalar amount)
{
    Core::Vector3 pos = getPosition();
    Core::Vector3 right = getRightVector();

    setPosition(pos - amount * right);
}

inline void Camera::walkBackward(Scalar amount)
{
    Core::Vector3 pos = getPosition();
    Core::Vector3 front = getDirection();

    setPosition(pos - amount * front);
}

inline void Camera::walkForward(Scalar amount)
{
    Core::Vector3 pos = getPosition();
    Core::Vector3 front = getDirection();

    setPosition(pos + amount * front);
}

inline void Camera::moveUpward(Scalar amount)
{
    Core::Vector3 pos = getPosition();
    Core::Vector3 up = getUpVector();

    setPosition(pos + amount * up);
}

inline void Camera::moveDownward(Scalar amount)
{
    Core::Vector3 pos = getPosition();
    Core::Vector3 up = getUpVector();

    setPosition(pos - amount * up);
}

inline void Camera::rotateUp(Scalar amount)
{
    Core::Transform t(Core::AngleAxis(amount, getRightVector()));
    applyTransform(t);
}

inline void Camera::rotateDown(Scalar amount)
{
    Core::Transform t(Core::AngleAxis(-amount, getRightVector()));
    applyTransform(t);
}

inline void Camera::rotateLeft(Scalar amount)
{
    Core::Transform t(Core::AngleAxis(amount, Core::Vector3(0, 1, 0)));
    applyTransform(t);
}

inline void Camera::rotateRight(Scalar amount)
{
    Core::Transform t(Core::AngleAxis(-amount, Core::Vector3(0, 1, 0)));
    applyTransform(t);
}

inline void Camera::zoomIn(Scalar amount)
{
    setZoomFactor(getZoomFactor() - amount);
}

inline void Camera::zoomOut(Scalar amount)
{
    setZoomFactor(getZoomFactor() + amount);
}

} // End of Engine
} // End of Ra

