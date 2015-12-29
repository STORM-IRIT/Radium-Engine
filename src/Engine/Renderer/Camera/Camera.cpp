#include <Engine/Renderer/Camera/Camera.hpp>

#include <iostream>

#include <Core/Math/Math.hpp>

namespace Ra
{

    using Core::Math::Pi;
    using Core::Math::PiDiv2;
    using Core::Math::PiDiv4;

    namespace Engine
    {

        /// -------------------- ///
        /// CONSTRUCTOR
        /// -------------------- ///
        Camera::Camera( Scalar height, Scalar width )
            : m_frame( Core::Transform::Identity() )
            , m_projMatrix( Core::Matrix4::Identity() )
            , m_fov( PiDiv4 )
            , m_zNear( Scalar( 0.1f ) )
            , m_zFar( Scalar( 1000.0f ) )
            , m_zoomFactor( Scalar( 1.0f ) )
            , m_width( width )
            , m_height( height )
            , m_aspect( width / height )
            , m_projType( ProjType::PERSPECTIVE )
        {
        }

        /// -------------------- ///
        /// DESTRUCTOR
        /// -------------------- ///
        Camera::~Camera() { }

        /// -------------------- ///
        /// FRAME
        /// -------------------- ///
        void Camera::applyTransform( const Core::Transform& T )
        {

            Core::Transform t1 = Core::Transform::Identity();
            Core::Transform t2 = Core::Transform::Identity();
            t1.translation() = -getPosition();
            t2.translation() = getPosition();

            m_frame = t2 * T * t1 * m_frame;
        }

        /// -------------------- ///
        /// PROJECTION MATRIX
        /// -------------------- ///
        void Camera::updateProjMatrix()
        {


            switch ( m_projType )
            {
                case ProjType::ORTHOGRAPHIC:
                {
                    const Scalar dx = m_zoomFactor * 0.5f;
                    const Scalar dy = m_height * dx / m_width;
                    // ------------
                    // Compute projection matrix as describe in the doc of gluPerspective()
                    const Scalar l = -dx; // left
                    const Scalar r =  dx; // right
                    const Scalar t =  dy; // top
                    const Scalar b = -dy; // bottom

                    Core::Vector3 tr;
                    tr( 0 ) = - ( r + l ) / ( r - l );
                    tr( 1 ) = - ( t + b ) / ( t - b );
                    tr( 2 ) = - ( ( m_zFar + m_zNear ) / ( m_zFar - m_zNear ) );

                    m_projMatrix.setIdentity();

                    m_projMatrix.coeffRef( 0, 0 ) = 2.0f / ( r - l );
                    m_projMatrix.coeffRef( 1, 1 ) = 2.0f / ( t - b );
                    m_projMatrix.coeffRef( 2, 2 ) = -2.0f / ( m_zFar - m_zNear );
                    m_projMatrix.block<3, 1> ( 0, 3 ) = tr;

                }
                break;

                case ProjType::PERSPECTIVE:
                {
                    // Compute projection matrix as describe in the doc of gluPerspective()
                    const Scalar f     = std::tan( ( PiDiv2 ) - ( m_fov * m_zoomFactor * Scalar( 0.5 ) ) );
                    const Scalar ratio = m_width / m_height;
                    const Scalar diff  = m_zNear - m_zFar;

                    m_projMatrix.setZero();

                    m_projMatrix.coeffRef( 0, 0 ) = f / ratio;
                    m_projMatrix.coeffRef( 1, 1 ) = f;
                    m_projMatrix.coeffRef( 2, 2 ) = ( m_zFar + m_zNear ) / diff;
                    m_projMatrix.coeffRef( 2, 3 ) = ( Scalar( 2.0 ) * m_zFar * m_zNear ) / diff;
                    m_projMatrix.coeffRef( 3, 2 ) = Scalar( -1.0 );

                }
                break;

                default:
                    break;
            }
        }

        Core::Ray Camera::getRayFromScreen(const Core::Vector2& pix) const
        {
            // Ray starts from the camera's current position.
            return Core::Ray::Through(getPosition(), unProject(pix));
        }
    } // End of Engine
} // End of Ra
