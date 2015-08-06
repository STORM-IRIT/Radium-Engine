#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{
    namespace Engine
    {

        inline Core::Transform Camera::getFrame() const
        {
            return m_frame;
        }

        inline void Camera::setFrame ( const Core::Transform& frame )
        {
            m_frame = frame;
        }

        inline Core::Vector3 Camera::getPosition() const
        {
            return ( m_frame.translation() );
        }

        inline void Camera::setPosition ( const Core::Vector3& position )
        {
            m_frame.translation() = position;
        }

        inline Core::Vector3 Camera::getDirection() const
        {
            return ( -m_frame.affine().block<3, 1> ( 0, 2 ) );
        }

        inline void Camera::setDirection ( const Core::Vector3& direction )
        {
            Core::Transform T = Core::Transform::Identity();

            // Special case if two directions are exactly opposites we constrain.
            // to rotate around the up vector.
            if ( getDirection().cross ( direction ).squaredNorm() ==  0.f
                 && getDirection().dot ( direction ) < 0.f )
            {
                T.rotate ( Core::AngleAxis ( Core::Math::PiDiv2, getUpVector() ) );
            }
            else
            {
                T.rotate ( Core::Quaternion::FromTwoVectors ( getDirection(), direction ) );
            }
            applyTransform ( T );
        }

        inline Core::Vector3 Camera::getUpVector() const
        {
            return ( m_frame.affine().block<3, 1> ( 0, 1 ) );
        }

        inline void Camera::setUpVector ( const Core::Vector3& upVector )
        {
            Core::Transform T = Core::Transform::Identity();
            T.rotate ( Core::Quaternion::FromTwoVectors ( getUpVector(), upVector ) );
            applyTransform ( T );
        }

        inline Core::Vector3 Camera::getRightVector() const
        {
            return ( m_frame.affine().block<3, 1> ( 0, 0 ) );
        }

        inline Scalar Camera::getFOV() const
        {
            return m_fov;
        }

        inline void Camera::setFOV ( const Scalar fov )
        {
            m_fov = fov;
            updateProjMatrix();
        }

        inline Scalar Camera::getZNear() const
        {
            return m_zNear;
        }

        inline void Camera::setZNear ( const Scalar zNear )
        {
            m_zNear = zNear;
            updateProjMatrix();
        }

        inline Scalar Camera::getZFar() const
        {
            return m_zFar;
        }

        inline void Camera::setZFar ( const Scalar zFar )
        {
            m_zFar = zFar;
            updateProjMatrix();
        }

        inline Scalar Camera::getZoomFactor() const
        {
            return m_zoomFactor;
        }

        inline void Camera::setZoomFactor ( const Scalar& zoomFactor )
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

        inline void Camera::resize ( Scalar width, Scalar height )
        {
            m_width = width;
            m_height = height;
            updateProjMatrix();
        }

        inline Camera::ProjType Camera::getProjType() const
        {
            return m_projType;
        }

        inline void Camera::setProjType ( const ProjType& projectionType )
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

    } // End of Engine
} // End of Ra

