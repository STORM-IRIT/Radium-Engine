#include <Engine/Renderer/Camera/Camera.hpp>

#include <cmath>
#include <iostream>

namespace Ra {
namespace Engine {


/// -------------------- ///
/// CONSTRUCTOR
/// -------------------- ///
Camera::Camera()
    : m_frame     ( Core::Transform::Identity() )
    , m_projMatrix( Core::Matrix4::Identity()   )
    , m_fov       ( Scalar(M_PI / 4.0f )        )
    , m_zNear     ( Scalar(0.1f)                )
    , m_zFar      ( Scalar(1000.0f)             )
    , m_zoomFactor( Scalar(1.0f)                )
    , m_projType  ( ProjType::PERSPECTIVE       )
{
    updateViewMatrix();
}

// FIXME: = default ?
Camera::Camera( const Camera& cam )
    : m_frame     ( cam.m_frame      )
    , m_projMatrix( cam.m_projMatrix )
    , m_fov       ( cam.m_fov        )
    , m_zNear     ( cam.m_zNear      )
    , m_zFar      ( cam.m_zFar       )
    , m_zoomFactor( cam.m_zoomFactor )
    , m_projType  ( cam.m_projType   )
{}

/// -------------------- ///
/// DESTRUCTOR
/// -------------------- ///
Camera::~Camera() { }

/// -------------------- ///
/// FRAME
/// -------------------- ///
void Camera::applyTransform( const Core::Transform& T)
{

    Core::Transform t1 = Core::Transform::Identity();
    Core::Transform t2 = Core::Transform::Identity();
    t1.translation() = -getPosition();
    t2.translation() = getPosition();

    m_frame = t2 * T * t1* m_frame;
}


/// -------------------- ///
/// VIEW MATRIX
/// -------------------- ///
void Camera::updateViewMatrix()
{
    m_viewMatrix = m_frame.inverse().matrix();
}

/// -------------------- ///
/// PROJECTION MATRIX
/// -------------------- ///
void Camera::updateProjMatrix( const Scalar& width, const Scalar& height ) {

    switch( m_projType ) {
        case ProjType::ORTHOGRAPHIC: {
            const Scalar dx = m_zoomFactor * 0.5f;
            const Scalar dy = height * dx / width;
            // ------------
            // Compute projection matrix as describe in the doc of gluPerspective()
            const Scalar l = -dx; // left
            const Scalar r =  dx; // right
            const Scalar t =  dy; // top
            const Scalar b = -dy; // bottom

            Core::Vector3 tr;
            tr( 0 ) = -( r + l ) / ( r - l );
            tr( 1 ) = -( t + b ) / ( t - b );
            tr( 2 ) = -( ( m_zFar + m_zNear ) / ( m_zFar - m_zNear ) );

            m_projMatrix.setIdentity();

            m_projMatrix.coeffRef( 0, 0 ) = 2.0f / ( r - l );
            m_projMatrix.coeffRef( 1, 1 ) = 2.0f / ( t - b );
            m_projMatrix.coeffRef( 2, 2 ) = -2.0f / ( m_zFar - m_zNear );
            m_projMatrix.block< 3, 1 >( 0, 3 ) = tr;

        } break;

        case ProjType::PERSPECTIVE: {
            // Compute projection matrix as describe in the doc of gluPerspective()
            const Scalar f     = std::tan( ( Scalar(M_PI) * 0.5f ) - ( m_fov * m_zoomFactor * 0.5f ) );
            const Scalar ratio = width / height;
            const Scalar diff  = m_zNear - m_zFar;

            m_projMatrix.setZero();

            m_projMatrix.coeffRef( 0, 0 ) = f / ratio;
            m_projMatrix.coeffRef( 1, 1 ) = f;
            m_projMatrix.coeffRef( 2, 2 ) = ( m_zFar + m_zNear ) / diff;
            m_projMatrix.coeffRef( 2, 3 ) = ( 2.0f * m_zFar * m_zNear ) / diff;
            m_projMatrix.coeffRef( 3, 2 ) = -1.0f;

        } break;

        default: break;
    }
}

} // End of Engine
} // End of Ra
