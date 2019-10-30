#include <GuiBase/Viewer/Gizmo/TranslateGizmo.hpp>

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

namespace Ra {
namespace Gui {

const std::string colorAttribName = Engine::Mesh::getAttribName( Engine::Mesh::VERTEX_COLOR );

TranslateGizmo::TranslateGizmo( Engine::Component* c,
                                const Core::Transform& worldTo,
                                const Core::Transform& t,
                                Mode mode ) :
    Gizmo( c, worldTo, t, mode ),
    m_startPoint( Core::Vector3::Zero() ),
    m_initialPix( Core::Vector2::Zero() ),
    m_selectedAxis( -1 ),
    m_selectedPlane( -1 ) {
    constexpr Scalar arrowScale = .1_ra;
    constexpr Scalar axisWidth  = .05_ra;
    constexpr Scalar arrowFrac  = .15_ra;

    std::shared_ptr<Engine::RenderTechnique> rt( new Engine::RenderTechnique );
    rt->setConfiguration( Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );
    rt->resetMaterial( new Ra::Engine::BlinnPhongMaterial( "Default material" ) );

    // For x,y,z
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Utils::Color arrowColor = Core::Utils::Color::Black();
        arrowColor[i]                 = 1_ra;

        Core::Vector3 cylinderEnd = Core::Vector3::Zero();
        Core::Vector3 arrowEnd    = Core::Vector3::Zero();
        cylinderEnd[i]            = ( 1_ra - arrowFrac );
        arrowEnd[i]               = 1_ra;

        Core::Geometry::TriangleMesh cylinder =
            Core::Geometry::makeCylinder( Core::Vector3::Zero(),
                                          arrowScale * cylinderEnd,
                                          arrowScale * axisWidth / 2_ra,
                                          32,
                                          arrowColor );

        Core::Geometry::TriangleMesh cone = Core::Geometry::makeCone( arrowScale * cylinderEnd,
                                                                      arrowScale * arrowEnd,
                                                                      arrowScale * arrowFrac / 2_ra,
                                                                      32,
                                                                      arrowColor );

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

    for ( uint i = 0; i < 3; ++i )
    {
        Core::Utils::Color planeColor = Core::Utils::Color::Black();
        planeColor[i]                 = 1_ra;

        Core::Vector3 axis                        = Core::Vector3::Zero();
        axis[( i == 0 ? 1 : ( i == 1 ? 0 : 2 ) )] = 1;
        Core::Transform T                         = Core::Transform::Identity();
        T.rotate( Core::AngleAxis( Core::Math::PiDiv2, axis ) );
        T.translation()[( i + 1 ) % 3] += arrowScale / 8_ra * 3_ra;
        T.translation()[( i + 2 ) % 3] += arrowScale / 8_ra * 3_ra;

        Core::Geometry::TriangleMesh plane = Core::Geometry::makePlaneGrid(
            1, 1, Core::Vector2( arrowScale / 8_ra, arrowScale / 8_ra ), T, planeColor );

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

    updateTransform( mode, m_worldTo, m_transform );
}

void TranslateGizmo::updateTransform( Gizmo::Mode mode,
                                      const Core::Transform& worldTo,
                                      const Core::Transform& t ) {
    m_mode                           = mode;
    m_worldTo                        = worldTo;
    m_transform                      = t;
    Core::Transform displayTransform = Core::Transform::Identity();
    displayTransform.translate( m_transform.translation() );
    if ( m_mode == LOCAL )
    {
        Core::Matrix3 R = m_transform.rotation();
        R.col( 0 ).normalize();
        R.col( 1 ).normalize();
        R.col( 2 ).normalize();
        displayTransform.rotate( R );
    }

    for ( auto roIdx : roIds() )
    {
        Engine::RadiumEngine::getInstance()
            ->getRenderObjectManager()
            ->getRenderObject( roIdx )
            ->setLocalTransform( m_worldTo * displayTransform );
    }
}

void TranslateGizmo::selectConstraint( int drawableIdx ) {
    // reColor constraint
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
            else
            {
                m_selectedPlane = int( i - 3 );
                roMeshes()[size_t( ( m_selectedPlane + 1 ) % 3 )]->getCoreGeometry().colorize(
                    Core::Utils::Color::Yellow() );
                roMeshes()[size_t( ( m_selectedPlane + 2 ) % 3 )]->getCoreGeometry().colorize(
                    Core::Utils::Color::Yellow() );
                roMeshes()[size_t( m_selectedPlane + 3 )]->getCoreGeometry().colorize(
                    Core::Utils::Color::Yellow() );
            }
        }
    }

    for ( auto mesh : roMeshes() )
        mesh->setDirty( Engine::Mesh::VERTEX_COLOR );
}

Core::Transform TranslateGizmo::mouseMove( const Engine::Camera& cam,
                                           const Core::Vector2& nextXY,
                                           bool stepped,
                                           bool whole ) {
    static const Scalar step = .2_ra;

    if ( m_selectedAxis == -1 && m_selectedPlane == -1 ) return m_transform;

    // Get gizmo center and translation axis / plane normal
    std::vector<Scalar> hits;
    int axis                   = std::max( m_selectedAxis, m_selectedPlane );
    const Core::Vector3 origin = m_transform.translation();
    const Core::Vector3 translateDir =
        m_mode == LOCAL ? Core::Vector3( m_transform.rotation() * Core::Vector3::Unit( axis ) )
                        : Core::Vector3::Unit( axis );

    // Project the clicked points against the axis defined by the translation axis,
    // or the planes defined by the translation plane.
    Ra::Core::Vector3 endPoint;
    bool found = false;
    if ( m_selectedAxis > -1 )
    {
        found = findPointOnAxis( cam, origin, translateDir, m_initialPix + nextXY, endPoint, hits );
    }
    else if ( m_selectedPlane > -1 )
    {
        found =
            findPointOnPlane( cam, origin, translateDir, m_initialPix + nextXY, endPoint, hits );
    }

    if ( found )
    {
        // Initialize translation
        if ( !m_start )
        {
            m_start        = true;
            m_startPoint   = endPoint;
            m_initialTrans = origin;
        }

        // Apply translation
        Ra::Core::Vector3 tr = endPoint - m_startPoint;
        if ( stepped ) { tr = int( tr.norm() / step ) * step * tr.normalized(); }
        m_transform.translation() = m_initialTrans + tr;
    }

    return m_transform;
}

void TranslateGizmo::setInitialState( const Engine::Camera& /*cam*/,
                                      const Core::Vector2& /*initialXY*/ ) {
    m_initialPix = Core::Vector2::Zero();
    m_start      = false;
}

} // namespace Gui
} // namespace Ra
