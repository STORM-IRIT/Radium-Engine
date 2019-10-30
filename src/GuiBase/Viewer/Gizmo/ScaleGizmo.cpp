#include <GuiBase/Viewer/Gizmo/ScaleGizmo.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Utils/Color.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <GuiBase/Utils/Keyboard.hpp>

namespace Ra {
namespace Gui {

using namespace Ra::Core::Utils;

const std::string colorAttribName = Engine::Mesh::getAttribName( Engine::Mesh::VERTEX_COLOR );

ScaleGizmo::ScaleGizmo( Engine::Component* c,
                        const Core::Transform& worldTo,
                        const Core::Transform& t,
                        Mode mode ) :
    Gizmo( c, worldTo, t, mode ),
    m_prevScale( Core::Vector3::Ones() ),
    m_startPoint( Core::Vector3::Zero() ),
    m_initialPix( Core::Vector2::Zero() ),
    m_selectedAxis( -1 ),
    m_selectedPlane( -1 ) {
    constexpr Scalar arrowScale = .1_ra;
    constexpr Scalar axisWidth  = .05_ra;
    constexpr Scalar arrowFrac  = .125_ra;
    constexpr Scalar radius     = arrowScale * axisWidth / 2_ra;

    std::shared_ptr<Engine::RenderTechnique> rt( new Engine::RenderTechnique );
    rt->setConfiguration( Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );
    rt->resetMaterial( new Engine::BlinnPhongMaterial( "Default material" ) );

    // For x,y,z
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Utils::Color arrowColor = Core::Utils::Color::Black();
        arrowColor[i]                 = 1_ra;

        Core::Vector3 cylinderEnd = Core::Vector3::Zero();
        cylinderEnd[i]            = ( 1_ra - arrowFrac );

        Core::Geometry::TriangleMesh cylinder = Core::Geometry::makeCylinder(
            Core::Vector3::Zero(), arrowScale * cylinderEnd, radius, 32, arrowColor );

        Core::Aabb box;
        box.extend( Core::Vector3( -radius * 2_ra, -radius * 2_ra, -radius * 2_ra ) );
        box.extend( Core::Vector3( radius * 2_ra, radius * 2_ra, radius * 2_ra ) );
        box.translate( arrowScale * cylinderEnd );
        Core::Geometry::TriangleMesh cone = Core::Geometry::makeSharpBox( box, arrowColor );

        // Merge the cylinder and the cone to create the arrow shape.
        cylinder.append( cone );

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Arrow" ) );
        mesh->loadGeometry( std::move( cylinder ) );

        Engine::RenderObject* arrowDrawable =
            new Engine::RenderObject( "Gizmo Arrow", m_comp, Engine::RenderObjectType::UI );
        arrowDrawable->setRenderTechnique( rt );
        arrowDrawable->setMesh( mesh );

        addRenderObject( arrowDrawable, mesh );
    }

    // For xy,yz,zx
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Utils::Color planeColor = Core::Utils::Color::Black();
        planeColor[i]                 = 1_ra;

        Core::Vector3 axis                        = Core::Vector3::Zero();
        axis[( i == 0 ? 1 : ( i == 1 ? 0 : 2 ) )] = 1;
        Core::Transform T                         = Core::Transform::Identity();
        T.rotate( Core::AngleAxis( Core::Math::PiDiv2, axis ) );
        T.translation()[( i + 1 ) % 3] += arrowFrac * arrowScale * 3;
        T.translation()[( i + 2 ) % 3] += arrowFrac * arrowScale * 3;

        Core::Geometry::TriangleMesh plane = Core::Geometry::makePlaneGrid(
            1, 1, Core::Vector2( arrowFrac * arrowScale, arrowFrac * arrowScale ), T, planeColor );

        // \FIXME this hack is here to be sure the plane will be selected (see shader)
        auto& normals = plane.normalsWithLock();
        normals.getMap().fill( 0_ra );
        plane.normalsUnlock();

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Plane" ) );
        mesh->loadGeometry( std::move( plane ) );

        Engine::RenderObject* planeDrawable =
            new Engine::RenderObject( "Gizmo Plane", m_comp, Engine::RenderObjectType::UI );
        planeDrawable->setRenderTechnique( rt );
        planeDrawable->setMesh( mesh );

        addRenderObject( planeDrawable, mesh );
    }

    // update all
    updateTransform( mode, m_worldTo, m_transform );
}

void ScaleGizmo::updateTransform( Gizmo::Mode mode,
                                  const Core::Transform& worldTo,
                                  const Core::Transform& t ) {
    m_mode                           = mode;
    m_worldTo                        = worldTo;
    m_transform                      = t;
    Core::Transform displayTransform = Core::Transform::Identity();
    displayTransform.translate( m_transform.translation() );
    // always scale in LOCAL frame
    {
        Core::Matrix3 R = m_transform.rotation();
        R.col( 0 ).normalize();
        R.col( 1 ).normalize();
        R.col( 2 ).normalize();
        displayTransform.rotate( R );
    }

    for ( const auto& roIdx : roIds() )
    {
        Engine::RadiumEngine::getInstance()
            ->getRenderObjectManager()
            ->getRenderObject( roIdx )
            ->setLocalTransform( m_worldTo * displayTransform );
    }
}

void ScaleGizmo::selectConstraint( int drawableIdx ) {
    // reColor constraints
    roMeshes()[0]->getCoreGeometry().colorize( Core::Utils::Color::Red() );
    roMeshes()[1]->getCoreGeometry().colorize( Core::Utils::Color::Green() );
    roMeshes()[2]->getCoreGeometry().colorize( Core::Utils::Color::Blue() );
    roMeshes()[3]->getCoreGeometry().colorize( Core::Utils::Color::Red() );
    roMeshes()[4]->getCoreGeometry().colorize( Core::Utils::Color::Green() );
    roMeshes()[5]->getCoreGeometry().colorize( Core::Utils::Color::Blue() );

    // prepare selection
    m_selectedAxis  = -1;
    m_selectedPlane = -1;
    if ( drawableIdx >= 0 )
    {
        auto found =
            std::find( roIds().cbegin(), roIds().cend(), Core::Utils::Index( drawableIdx ) );
        if ( found != roIds().cend() )
        {
            auto i = std::distance( roIds().cbegin(), found );
            if ( i < 3 )
            {
                m_selectedAxis = int( i );
                roMeshes()[size_t( m_selectedAxis )]->getCoreGeometry().colorize(
                    Core::Utils::Color::Yellow() );
            }
            else if ( i < 6 )
            {
                m_selectedPlane = int( i ) - 3;
                roMeshes()[size_t( ( m_selectedPlane + 1 ) % 3 )]->getCoreGeometry().colorize(
                    Core::Utils::Color::Yellow() );
                roMeshes()[size_t( ( m_selectedPlane + 2 ) % 3 )]->getCoreGeometry().colorize(
                    Core::Utils::Color::Yellow() );
                roMeshes()[size_t( m_selectedPlane + 3 )]->getCoreGeometry().colorize(
                    Core::Utils::Color::Yellow() );
            }
        }
    }

    for ( const auto& mesh : roMeshes() )
        mesh->setDirty( Engine::Mesh::VERTEX_COLOR );
}

Core::Transform ScaleGizmo::mouseMove( const Engine::Camera& cam,
                                       const Core::Vector2& nextXY,
                                       bool stepped,
                                       bool whole ) {
    static const Scalar step = .2_ra;

    // Recolor gizmo

    if ( whole )
    {
        for ( const auto& mesh : roMeshes() )
            mesh->getCoreGeometry().colorize( Core::Utils::Color::Yellow() );
    }
    else
    {
        roMeshes()[0]->getCoreGeometry().colorize( Core::Utils::Color::Red() );
        roMeshes()[1]->getCoreGeometry().colorize( Core::Utils::Color::Green() );
        roMeshes()[2]->getCoreGeometry().colorize( Core::Utils::Color::Blue() );
        roMeshes()[3]->getCoreGeometry().colorize( Core::Utils::Color::Red() );
        roMeshes()[4]->getCoreGeometry().colorize( Core::Utils::Color::Green() );
        roMeshes()[5]->getCoreGeometry().colorize( Core::Utils::Color::Blue() );

        if ( m_selectedAxis > -1 )
        {
            roMeshes()[size_t( m_selectedAxis )]->getCoreGeometry().colorize(
                Core::Utils::Color::Yellow() );
        }
        if ( m_selectedPlane > -1 )
        {
            roMeshes()[size_t( ( m_selectedPlane + 1 ) % 3 )]->getCoreGeometry().colorize(
                Core::Utils::Color::Yellow() );
            roMeshes()[size_t( ( m_selectedPlane + 2 ) % 3 )]->getCoreGeometry().colorize(
                Core::Utils::Color::Yellow() );
            roMeshes()[size_t( m_selectedPlane + 3 )]->getCoreGeometry().colorize(
                Core::Utils::Color::Yellow() );
        }
    }

    for ( const auto& mesh : roMeshes() )
        mesh->setDirty( Engine::Mesh::VERTEX_COLOR );

    if ( m_selectedAxis == -1 && m_selectedPlane == -1 && !whole ) { return m_transform; }

    // Get gizmo center and translation axis / plane normal
    int dir                    = whole ? 0 : std::max( m_selectedAxis, m_selectedPlane );
    const Core::Vector3 origin = m_transform.translation();
    Core::Vector3 scaleDir = Core::Vector3( m_transform.rotation() * Core::Vector3::Unit( dir ) );

    // Project the clicked points against the axis defined by the scale axis,
    // or the planes defined by the scale plane.
    std::vector<Scalar> hits;
    bool found = false;
    Core::Vector3 endPoint;
    if ( whole )
    { found = findPointOnPlane( cam, origin, scaleDir, m_initialPix + nextXY, endPoint, hits ); }
    else if ( m_selectedAxis > -1 )
    { found = findPointOnAxis( cam, origin, scaleDir, m_initialPix + nextXY, endPoint, hits ); }
    else if ( m_selectedPlane >= 0 )
    { found = findPointOnPlane( cam, origin, scaleDir, m_initialPix + nextXY, endPoint, hits ); }
    if ( !found ) { return m_transform; }

    // Initialize scale
    if ( !m_start )
    {
        m_start      = true;
        m_startPos   = m_transform.translation();
        m_prevScale  = Core::Vector3::Ones();
        m_startPoint = endPoint;
    }

    // Prevent scale == 0
    const Scalar a = ( endPoint - m_startPos ).squaredNorm();
    if ( a < 1e-3_ra ) { return m_transform; }

    // Get scale value
    const Core::Vector3 b = m_startPoint - m_startPos;
    Scalar scale          = std::sqrt( a ) / b.norm();
    if ( stepped ) { scale = int( scale / step + 1 ) * step; }

    // Apply scale
    m_transform.scale( m_prevScale );
    if ( whole ) { m_prevScale = scale * Core::Vector3::Ones(); }
    else if ( m_selectedAxis > -1 )
    {
        m_prevScale                 = Core::Vector3::Ones();
        m_prevScale[m_selectedAxis] = scale;
    }
    else if ( m_selectedPlane >= 0 )
    {
        m_prevScale                              = Core::Vector3::Ones();
        m_prevScale[( m_selectedPlane + 1 ) % 3] = scale;
        m_prevScale[( m_selectedPlane + 2 ) % 3] = scale;
    }
    m_transform.scale( m_prevScale );
    m_prevScale = m_prevScale.cwiseInverse();

    return m_transform;
}

void ScaleGizmo::setInitialState( const Engine::Camera& /*cam*/,
                                  const Core::Vector2& /*initialXY*/ ) {
    m_initialPix = Core::Vector2::Zero();
    m_start      = false;
}

} // namespace Gui
} // namespace Ra
