#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{
    namespace Engine
    {

        inline Core::Transform Camera::getFrame() const
        {
            return m_frame;
        }

        inline void Camera::setFrame( const Core::Transform& frame )
        {
            m_frame = frame;
        }

        inline Core::Vector3 Camera::getPosition() const
        {
            return ( m_frame.translation() );
        }

        inline void Camera::setPosition( const Core::Vector3& position )
        {
            m_frame.translation() = position;
        }

        inline Core::Vector3 Camera::getDirection() const
        {
            return ( -m_frame.affine().block<3, 1> ( 0, 2 ) );
        }

        inline void Camera::setDirection( const Core::Vector3& direction )
        {
            Core::Transform T = Core::Transform::Identity();

            auto d = direction.normalized();

            // Special case if two directions are exactly opposites we constrain.
            // to rotate around the up vector.
            if ( getDirection().cross( d ).squaredNorm() ==  0.f
                 && getDirection().dot( d ) < 0.f )
            {
                T.rotate( Core::AngleAxis( Core::Math::PiDiv2, getUpVector() ) );
            }
            else
            {
                T.rotate( Core::Quaternion::FromTwoVectors( getDirection(), d ) );
            }
            applyTransform( T );
        }

        inline Core::Vector3 Camera::getUpVector() const
        {
            return ( m_frame.affine().block<3, 1> ( 0, 1 ) );
        }

        inline void Camera::setUpVector( const Core::Vector3& upVector )
        {
            Core::Transform T = Core::Transform::Identity();
            T.rotate( Core::Quaternion::FromTwoVectors( getUpVector(), upVector ) );
            applyTransform( T );
        }

        inline Core::Vector3 Camera::getRightVector() const
        {
            return ( m_frame.affine().block<3, 1> ( 0, 0 ) );
        }

        inline Scalar Camera::getFOV() const
        {
            return m_fov;
        }

        inline void Camera::setFOV( const Scalar fov )
        {
            m_fov = fov;
            updateProjMatrix();
        }

        inline Scalar Camera::getZNear() const
        {
            return m_zNear;
        }

        inline void Camera::setZNear( const Scalar zNear )
        {
            m_zNear = zNear;
            updateProjMatrix();
        }

        inline Scalar Camera::getZFar() const
        {
            return m_zFar;
        }

        inline void Camera::setZFar( const Scalar zFar )
        {
            m_zFar = zFar;
            updateProjMatrix();
        }

        inline Scalar Camera::getZoomFactor() const
        {
            return m_zoomFactor;
        }

        inline void Camera::setZoomFactor( const Scalar& zoomFactor )
        {
            m_zoomFactor = zoomFactor;
            updateProjMatrix();
        }

        inline Scalar Camera::getWidth() const
        {
            return m_width;
        }

        inline Scalar Camera::getHeight() const
        {
            return m_height;
        }

        inline void Camera::resize( Scalar width, Scalar height )
        {
            m_width = width;
            m_height = height;
            m_aspect = width / height;
            updateProjMatrix();
        }

        inline Scalar Camera::getAspect() const
        {
            return m_aspect;
        }

        inline Camera::ProjType Camera::getProjType() const
        {
            return m_projType;
        }

        inline void Camera::setProjType( const ProjType& projectionType )
        {
            m_projType = projectionType;
        }

        inline Core::Matrix4 Camera::getViewMatrix() const
        {
            return m_frame.inverse().matrix();
        }

        inline Core::Matrix4 Camera::getProjMatrix() const
        {
            return m_projMatrix;
        }

        inline Core::Vector2 Camera::project(const Core::Vector3& p) const
        {
            Core::Vector4 point = Core::Vector4::Ones();
            point.head<3>() = p;
            auto vpPoint = getProjMatrix() * getViewMatrix() * point;

            return Core::Vector2( m_width  * 0.5f * (vpPoint.x() + 1),
                                  m_height * 0.5f *  (1 - vpPoint.y()));

        }

        inline Core::Vector3 Camera::unProject(const Core::Vector2& pix) const
        {
            const Scalar localX = (2.f* pix.x()) / m_width - 1;
            // Y is "inverted" (goes downwards)
            const Scalar localY = -(2.f* pix.y()) / m_height + 1;

            // Multiply the point in screen space by the inverted projection matrix
            // and then by the inverted view matrix ( = m_frame) to get it in world space.
            // NB : localPoint needs to be a vec4 to be multiplied by the proj matrix.
            const Core::Vector4 localPoint (localX, localY, -m_zNear, 1.f);
            auto unproj = getProjMatrix().inverse()*localPoint;
            return m_frame *unproj.head<3>();
        }
    } // End of Engine
} // End of Ra

