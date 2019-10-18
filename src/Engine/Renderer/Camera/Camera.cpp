#include <Engine/Renderer/Camera/Camera.hpp>

#include <Core/Math/Math.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

namespace Ra {

using Core::Math::Pi;
using Core::Math::PiDiv2;
using Core::Math::PiDiv4;

namespace Engine {

Camera::Camera( Entity* entity, const std::string& name, Scalar height, Scalar width ) :
    Component( name, entity ),
    m_width{width},
    m_height{height},
    m_aspect{width / height} {}

Camera::~Camera() = default;

void Camera::initialize() {
    if ( !m_renderObjects.empty() ) return;
    // Create the render mesh for the camera
    auto m = std::make_shared<Mesh>( m_name + "_mesh" );
    Ra::Core::Geometry::TriangleMesh triMesh;
    triMesh.setVertices( {{0_ra, 0_ra, 0_ra},
                          {-.5_ra, -.5_ra, -1_ra},
                          {-.5_ra, .5_ra, -1_ra},
                          {.5_ra, .5_ra, -1_ra},
                          {.5_ra, -.5_ra, -1_ra},
                          {-.3_ra, .5_ra, -1_ra},
                          {0_ra, .7_ra, -1_ra},
                          {.3_ra, .5_ra, -1_ra}} );
    triMesh.m_indices = {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 1}, {5, 6, 7}};
    Core::Vector4Array c( 8, {.2_ra, .2_ra, .2_ra, 1_ra} );
    triMesh.addAttrib( Mesh::getAttribName( Mesh::VERTEX_COLOR ), c );

    m->loadGeometry( std::move( triMesh ) );

    // Create the RO
    m_RO = RenderObject::createRenderObject( m_name + "_RO", this, RenderObjectType::Debug, m );
    m_RO->getRenderTechnique()->setConfiguration(
        ShaderConfigurationFactory::getConfiguration( "Plain" ) );
    m_RO->setLocalTransform( m_frame );
    addRenderObject( m_RO );
}

void Camera::show( bool on ) {
    CORE_ASSERT( m_RO, "Camera's render object must be initialize with Camera::intialize()" );
    m_RO->setVisible( on );
}

void Camera::applyTransform( const Core::Transform& T ) {
    CORE_ASSERT( m_RO, "Camera's render object must be initialize with Camera::intialize()" );

    Core::Transform t1 = Core::Transform::Identity();
    Core::Transform t2 = Core::Transform::Identity();
    t1.translation()   = -getPosition();
    t2.translation()   = getPosition();

    m_frame = t2 * T * t1 * m_frame;
    m_RO->setLocalTransform( m_frame );
}

void Camera::updateProjMatrix() {

    switch ( m_projType )
    {
    case ProjType::ORTHOGRAPHIC:
    {
        const Scalar dx = m_zoomFactor * .5_ra;
        const Scalar dy = dx / m_aspect;
        // ------------
        // Compute projection matrix as describe in the doc of gluPerspective()
        const Scalar l = -dx; // left
        const Scalar r = dx;  // right
        const Scalar t = dy;  // top
        const Scalar b = -dy; // bottom

        Core::Vector3 tr( -( r + l ) / ( r - l ),
                          -( t + b ) / ( t - b ),
                          -( ( m_zFar + m_zNear ) / ( m_zFar - m_zNear ) ) );

        m_projMatrix.setIdentity();

        m_projMatrix.coeffRef( 0, 0 )    = 2_ra / ( r - l );
        m_projMatrix.coeffRef( 1, 1 )    = 2_ra / ( t - b );
        m_projMatrix.coeffRef( 2, 2 )    = -2_ra / ( m_zFar - m_zNear );
        m_projMatrix.block<3, 1>( 0, 3 ) = tr;
    }
    break;

    case ProjType::PERSPECTIVE:
    {
        // Compute projection matrix as describe in the doc of gluPerspective()
        const Scalar f    = std::tan( ( PiDiv2 ) - ( m_fov * m_zoomFactor * .5_ra ) );
        const Scalar diff = m_zNear - m_zFar;

        m_projMatrix.setZero();

        m_projMatrix.coeffRef( 0, 0 ) = f / m_aspect;
        m_projMatrix.coeffRef( 1, 1 ) = f;
        m_projMatrix.coeffRef( 2, 2 ) = ( m_zFar + m_zNear ) / diff;
        m_projMatrix.coeffRef( 2, 3 ) = ( 2_ra * m_zFar * m_zNear ) / diff;
        m_projMatrix.coeffRef( 3, 2 ) = -1_ra;
    }
    break;

    default:
        break;
    }
}

Core::Ray Camera::getRayFromScreen( const Core::Vector2& pix ) const {
    // Ray starts from the camera's current position.
    return Core::Ray::Through( getPosition(), unProject( pix ) );
}

Camera* Camera::duplicate( Entity* cloneEntity, const std::string& cloneName ) const {
    auto cam          = new Camera( cloneEntity, cloneName, m_width, m_height );
    cam->m_frame      = m_frame;
    cam->m_projMatrix = m_projMatrix;
    cam->m_projType   = m_projType;
    cam->m_zoomFactor = m_zoomFactor;
    cam->m_fov        = m_fov;
    cam->m_zNear      = m_zNear;
    cam->m_zFar       = m_zFar;
    cam->initialize();
    return cam;
}

void Camera::fitZRange( const Core::Aabb& aabb ) {
    const auto& minAabb         = aabb.min();
    const auto& maxAabb         = aabb.max();
    const auto& position        = m_frame.translation();
    Ra::Core::Vector3 direction = m_frame.linear() * Ra::Core::Vector3( 0_ra, 0_ra, -1_ra );

    m_zNear = m_zFar = direction.dot( minAabb - position );

    auto adaptRange = [position, direction, this]( Scalar x, Scalar y, Scalar z ) {
        Ra::Core::Vector3 corner( x, y, z );
        auto d        = direction.dot( corner - position );
        this->m_zNear = std::min( d, this->m_zNear );
        this->m_zFar  = std::max( d, this->m_zFar );
    };

    adaptRange( minAabb[0], minAabb[1], maxAabb[2] );
    adaptRange( minAabb[0], maxAabb[1], minAabb[2] );
    adaptRange( minAabb[0], maxAabb[1], maxAabb[2] );
    adaptRange( maxAabb[0], maxAabb[1], maxAabb[2] );
    adaptRange( maxAabb[0], maxAabb[1], minAabb[2] );
    adaptRange( maxAabb[0], minAabb[1], maxAabb[2] );
    adaptRange( maxAabb[0], minAabb[1], minAabb[2] );

    // ensure a minimum depth range
    Scalar range = ( m_zFar - m_zNear ) / 100_ra;
    m_zNear      = std::max( range, m_zNear - range );
    m_zFar       = std::max( 2_ra * range, m_zFar + range );

    updateProjMatrix();
}

} // namespace Engine
} // namespace Ra
