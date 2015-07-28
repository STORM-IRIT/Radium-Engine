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
    , m_viewMatrix( Core::Matrix4::Identity()   )
    , m_projMatrix( Core::Matrix4::Identity()   )
    , m_fov       ( M_PI / 4.0                  )
    , m_zNear     ( 0.1                         )
    , m_zFar      ( 1000.0                      )
    , m_zoomFactor( 1.0                         )
    , m_projType  ( ProjType::PERSPECTIVE       )
{
    updateViewMatrix();
}

// FIXME: = default ?
Camera::Camera( const Camera& cam )
    : m_frame     ( cam.m_frame      )
    , m_viewMatrix( cam.m_viewMatrix )
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
    Core::Transform t1, t2;
    t1.setIdentity();
    t2.setIdentity();
    {
            t1.translation() = -getPosition();
            t2.translation() =  getPosition();
    }

    m_frame = t2 * T * t1 * m_frame;
}




/// -------------------- ///
/// VIEW MATRIX
/// -------------------- ///
void Camera::updateViewMatrix() {
    const Core::Vector3 e  = getPosition();
    const Core::Vector3 f  = getDirection().normalized();
    const Core::Vector3 up = getUpVector().normalized();

    const Core::Vector3 s = f.cross( up );
    const Core::Vector3 u = s.cross( f  );

    Core::Matrix4 T(Core::Matrix4::Identity());
    T.block< 3, 1 >( 0, 3 ) = -e;

    m_viewMatrix.setIdentity();
    m_viewMatrix.block< 1, 3 >( 0, 0 ) =  s;
    m_viewMatrix.block< 1, 3 >( 1, 0 ) =  u;
    m_viewMatrix.block< 1, 3 >( 2, 0 ) = -f;

    m_viewMatrix = m_viewMatrix * T;
        std::cout<<"======"<<std::endl;
        std::cout<<m_viewMatrix<<std::endl;
        std::cout<<m_frame.matrix()<<std::endl;

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
