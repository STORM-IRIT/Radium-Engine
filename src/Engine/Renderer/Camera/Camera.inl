
namespace Ra {
namespace Engine {

inline Core::Math::Transform Camera::getFrame() const {
    return m_frame;
}

inline void Camera::setFrame( const Core::Math::Transform& frame ) {
    m_frame = frame;
}

inline Core::Math::Vector3 Camera::getPosition() const {
    return ( m_frame.translation() );
}

inline void Camera::setPosition( const Core::Math::Vector3& position ) {
    m_frame.translation() = position;
}

inline Core::Math::Vector3 Camera::getDirection() const {
    return ( -m_frame.affine().block<3, 1>( 0, 2 ) ).normalized();
}

inline void Camera::setDirection( const Core::Math::Vector3& direction ) {
    Core::Math::Transform T = Core::Math::Transform::Identity();

    auto d0 = getDirection();
    auto d1 = direction.normalized();

    auto c = d0.cross( d1 );
    auto d = d0.dot( d1 );

    // Special case if two directions are exactly opposites we constrain.
    // to rotate around the up vector.
    if ( c.isApprox( Core::Math::Vector3::Zero() ) && d < 0.0 )
    {
        T.rotate( Core::Math::AngleAxis( Core::Math::PiDiv2, Core::Math::Vector3::UnitY() ) );
    } else
    { T.rotate( Core::Math::Quaternion::FromTwoVectors( d0, d1 ) ); }
    applyTransform( T );
}

inline Core::Math::Vector3 Camera::getUpVector() const {
    return ( m_frame.affine().block<3, 1>( 0, 1 ) );
}

inline void Camera::setUpVector( const Core::Math::Vector3& upVector ) {
    Core::Math::Transform T = Core::Math::Transform::Identity();
    T.rotate( Core::Math::Quaternion::FromTwoVectors( getUpVector(), upVector ) );
    applyTransform( T );
}

inline Core::Math::Vector3 Camera::getRightVector() const {
    return ( m_frame.affine().block<3, 1>( 0, 0 ) );
}

inline Scalar Camera::getFOV() const {
    return m_fov;
}

inline void Camera::setFOV( const Scalar fov ) {
    m_fov = fov;
    updateProjMatrix();
}

inline Scalar Camera::getZNear() const {
    return m_zNear;
}

inline void Camera::setZNear( const Scalar zNear ) {
    m_zNear = zNear;
    updateProjMatrix();
}

inline Scalar Camera::getZFar() const {
    return m_zFar;
}

inline void Camera::setZFar( const Scalar zFar ) {
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

inline void Camera::resize( Scalar width, Scalar height ) {
    m_width = width;
    m_height = height;
    m_aspect = width / height;
    updateProjMatrix();
}

inline Scalar Camera::getAspect() const {
    return m_aspect;
}

inline Camera::ProjType Camera::getProjType() const {
    return m_projType;
}

inline void Camera::setProjType( const ProjType& projectionType ) {
    m_projType = projectionType;
}

inline Core::Math::Matrix4 Camera::getViewMatrix() const {
    return m_frame.inverse().matrix();
}

inline Core::Math::Matrix4 Camera::getProjMatrix() const {
    return m_projMatrix;
}

inline Core::Math::Vector2 Camera::project( const Core::Math::Vector3& p ) const {
    Core::Math::Vector4 point = Core::Math::Vector4::Ones();
    point.head<3>() = p;
    auto vpPoint = getProjMatrix() * getViewMatrix() * point;

    return Core::Math::Vector2( m_width * 0.5f * ( vpPoint.x() + 1 ),
                          m_height * 0.5f * ( 1 - vpPoint.y() ) );
}

inline Core::Math::Vector3 Camera::unProject( const Core::Math::Vector2& pix ) const {
    const Scalar localX = ( 2.f * pix.x() ) / m_width - 1;
    // Y is "inverted" (goes downwards)
    const Scalar localY = -( 2.f * pix.y() ) / m_height + 1;

    // Multiply the point in screen space by the inverted projection matrix
    // and then by the inverted view matrix ( = m_frame) to get it in world space.
    // NB : localPoint needs to be a vec4 to be multiplied by the proj matrix.
    const Core::Math::Vector4 localPoint( localX, localY, -m_zNear, 1.f );
    const Core::Math::Vector4 unproj = getProjMatrix().inverse() * localPoint;
    return m_frame * unproj.head<3>();
}
} // namespace Engine
} // namespace Ra
