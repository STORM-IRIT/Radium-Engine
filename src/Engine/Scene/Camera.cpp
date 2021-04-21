#include <Engine/Scene/Camera.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Math/Math.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/PlainMaterial.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Rendering/RenderObject.hpp>

namespace Ra {

using Core::Math::Pi;
using Core::Math::PiDiv2;
using Core::Math::PiDiv4;

namespace Engine {

namespace Data {

Camera::Camera( Scalar width, Scalar height ) :
    m_width {width}, m_height {height}, m_aspect {width / height} {}

void Camera::setDirection( const Core::Vector3& direction ) {
    Core::Transform T = Core::Transform::Identity();

    auto d0 = getDirection();
    auto d1 = direction.normalized();

    auto c = d0.cross( d1 );
    auto d = d0.dot( d1 );

    // Special case if two directions are exactly opposites we constrain.
    // to rotate around the up vector.
    if ( c.isApprox( Core::Vector3::Zero() ) && d < 0.0 )
    { T.rotate( Core::AngleAxis( Core::Math::PiDiv2, Core::Vector3::UnitY() ) ); }
    else
    { T.rotate( Core::Quaternion::FromTwoVectors( d0, d1 ) ); }
    applyTransform( T );
}

void Camera::resize( Scalar width, Scalar height ) {
    m_width  = width;
    m_height = height;
    m_aspect = width / height;
    updateProjMatrix();
}

void Camera::applyTransform( const Core::Transform& T ) {
    Core::Transform t1 = Core::Transform::Identity();
    Core::Transform t2 = Core::Transform::Identity();
    t1.translation()   = -getPosition();
    t2.translation()   = getPosition();

    m_frame = t2 * T * t1 * m_frame;
}

void Camera::updateProjMatrix() {

    switch ( m_projType )
    {
    case ProjType::ORTHOGRAPHIC: {
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

    case ProjType::PERSPECTIVE: {
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

void Camera::fitZRange( const Core::Aabb& aabb ) {
    const auto& position        = m_frame.translation();
    Ra::Core::Vector3 direction = m_frame.linear() * Ra::Core::Vector3( 0_ra, 0_ra, -1_ra );

    const auto& minAabb = aabb.min();
    const auto& maxAabb = aabb.max();
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

    if ( m_zNear < m_minZNear ) { m_zNear = m_minZNear; }
    if ( ( m_zFar - m_zNear ) < m_minZRange ) { m_zFar += m_minZRange - ( m_zFar - m_zNear ); }

    updateProjMatrix();
}

Core::Ray Camera::getRayFromScreen( const Core::Vector2& pix ) const {
    // Ray starts from the camera's current position.
    return Core::Ray::Through( getPosition(), unProject( pix ) );
}

} // namespace Data

namespace Scene {

CameraComponent::CameraComponent( Entity* entity,
                                  const std::string& name,
                                  Scalar height,
                                  Scalar width ) :
    Component( name, entity ) {
    m_camera = std::make_unique<Data::Camera>( height, width );
}

CameraComponent::~CameraComponent() = default;

void CameraComponent::initialize() {
    using Data::Material;
    using Data::Mesh;
    using Data::PlainMaterial;
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
    triMesh.setIndices( {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 1}, {5, 6, 7}} );
    Core::Vector4Array c( 8, {.2_ra, .2_ra, .2_ra, 1_ra} );
    triMesh.addAttrib( Mesh::getAttribName( Mesh::VERTEX_COLOR ), c );

    m->loadGeometry( std::move( triMesh ) );

    // Create the RO
    auto mat = Core::make_shared<PlainMaterial>( m_name + "_Material" );
    /// \todo switch to "mat->m_color          = {.2_ra, .2_ra, .2_ra, 1_ra};"
    mat->m_perVertexColor = true;
    Rendering::RenderTechnique rt;
    auto cfg = Data::ShaderConfigurationFactory::getConfiguration( "Plain" );
    rt.setConfiguration( *cfg );
    rt.setParametersProvider( mat );
    m_RO = Rendering::RenderObject::createRenderObject(
        m_name + "_RO", this, Rendering::RenderObjectType::Debug, m, rt );
    m_RO->setLocalTransform( m_camera->getFrame() );
    m_RO->setMaterial( mat );
    show( false );
    addRenderObject( m_RO );
}

void CameraComponent::show( bool on ) {
    CORE_ASSERT( m_RO, "Camera's render object must be initialize with Camera::intialize()" );
    m_RO->setVisible( on );
}

void CameraComponent::applyTransform( const Core::Transform& T ) {
    CORE_ASSERT( m_RO, "Camera's render object must be initialize with Camera::intialize()" );
    m_camera->applyTransform( T );
    m_RO->setLocalTransform( m_camera->getFrame() );
}

void CameraComponent::updateProjMatrix() {
    m_camera->updateProjMatrix();
}

Core::Ray CameraComponent::getRayFromScreen( const Core::Vector2& pix ) const {
    // Ray starts from the camera's current position.
    return m_camera->getRayFromScreen( pix );
}

CameraComponent* CameraComponent::duplicate( Entity* cloneEntity,
                                             const std::string& cloneName ) const {
    auto cam =
        new CameraComponent {cloneEntity, cloneName, m_camera->getWidth(), m_camera->getHeight()};
    cam->getCamera()->setFrame( m_camera->getFrame() );
    cam->getCamera()->setProjMatrix( m_camera->getProjMatrix() );
    cam->getCamera()->setType( m_camera->getType() );
    cam->getCamera()->setZoomFactor( m_camera->getZoomFactor() );
    cam->getCamera()->setFOV( m_camera->getFOV() );
    cam->getCamera()->setZNear( m_camera->getZNear() );
    cam->getCamera()->setZFar( m_camera->getZFar() );
    cam->initialize();
    return cam;
}

void CameraComponent::fitZRange( const Core::Aabb& aabb ) {
    m_camera->fitZRange( aabb );
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
