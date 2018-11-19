#include <GuiBase/Viewer/Gizmo/TranslateGizmo.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Math/RayCast.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>

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

TranslateGizmo::TranslateGizmo( Engine::Component* c, const Core::Transform& worldTo,
                                const Core::Transform& t, Mode mode ) :
    Gizmo( c, worldTo, t, mode ),
    m_startPoint( Core::Vector3::Zero() ),
    m_initialPix( Core::Vector2::Zero() ),
    m_selectedAxis( -1 ),
    m_selectedPlane( -1 ) {
    constexpr Scalar arrowScale = 0.1f;
    constexpr Scalar axisWidth = 0.05f;
    constexpr Scalar arrowFrac = 0.15f;

    std::shared_ptr<Engine::RenderTechnique> rt( new Engine::RenderTechnique );
    rt->setConfiguration( Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );
    rt->resetMaterial( new Ra::Engine::BlinnPhongMaterial( "Default material" ) );

    // For x,y,z
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Vector3 cylinderEnd = Core::Vector3::Zero();
        Core::Vector3 arrowEnd = Core::Vector3::Zero();
        cylinderEnd[i] = ( 1.f - arrowFrac );
        arrowEnd[i] = 1.f;

        Core::TriangleMesh cylinder = Core::MeshUtils::makeCylinder(
            Core::Vector3::Zero(), arrowScale * cylinderEnd, arrowScale * axisWidth / 2.f );

        Core::TriangleMesh cone = Core::MeshUtils::makeCone(
            arrowScale * cylinderEnd, arrowScale * arrowEnd, arrowScale * arrowFrac / 2.f );

        // Merge the cylinder and the cone to create the arrow shape.
        cylinder.append( cone );

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Arrow" ) );
        mesh->loadGeometry( cylinder );
        Core::Color arrowColor = Core::Color::Zero();
        arrowColor[i] = 1.f;
        mesh->colorize( arrowColor );

        Engine::RenderObject* arrowDrawable =
            new Engine::RenderObject( "Gizmo Arrow", m_comp, Engine::RenderObjectType::UI );
        arrowDrawable->setRenderTechnique( rt );
        arrowDrawable->setMesh( mesh );

        m_renderObjects.push_back( m_comp->addRenderObject( arrowDrawable ) );
    }

    for ( uint i = 0; i < 3; ++i )
    {
        Core::Vector3 axis = Core::Vector3::Zero();
        axis[( i == 0 ? 1 : ( i == 1 ? 0 : 2 ) )] = 1;
        Core::Transform T = Core::Transform::Identity();
        T.rotate( Core::AngleAxis( Core::Math::PiDiv2, axis ) );
        T.translation()[( i + 1 ) % 3] += arrowScale / 8 * 3;
        T.translation()[( i + 2 ) % 3] += arrowScale / 8 * 3;

        Core::TriangleMesh plane = Core::MeshUtils::makePlaneGrid(
            1, 1, Core::Vector2( arrowScale / 8, arrowScale / 8 ), T );
        auto& n = plane.normals();
#pragma omp parallel for
        for ( int i = 0; i < n.size(); ++i )
        {
            n[i] = Core::Vector3::Zero();
        }

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Plane" ) );
        mesh->loadGeometry( plane );
        Core::Color planeColor = Core::Color::Zero();
        planeColor[i] = 1.f;
        mesh->colorize( planeColor );

        Engine::RenderObject* planeDrawable =
            new Engine::RenderObject( "Gizmo Plane", m_comp, Engine::RenderObjectType::UI );
        planeDrawable->setRenderTechnique( rt );
        planeDrawable->setMesh( mesh );
        m_renderObjects.push_back( m_comp->addRenderObject( planeDrawable ) );
    }

    updateTransform( mode, m_worldTo, m_transform );
}

void TranslateGizmo::updateTransform( Gizmo::Mode mode, const Core::Transform& worldTo,
                                      const Core::Transform& t ) {
    m_mode = mode;
    m_worldTo = worldTo;
    m_transform = t;
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

    for ( auto roIdx : m_renderObjects )
    {
        Engine::RadiumEngine::getInstance()
            ->getRenderObjectManager()
            ->getRenderObject( roIdx )
            ->setLocalTransform( m_worldTo * displayTransform );
    }
}

void TranslateGizmo::selectConstraint( int drawableIdx ) {
    // reColor constraint
    auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();
    auto RO = roMgr->getRenderObject( m_renderObjects[0] );
    RO->getMesh()->colorize( Core::Colors::Red() );
    RO = roMgr->getRenderObject( m_renderObjects[1] );
    RO->getMesh()->colorize( Core::Colors::Green() );
    RO = roMgr->getRenderObject( m_renderObjects[2] );
    RO->getMesh()->colorize( Core::Colors::Blue() );
    RO = roMgr->getRenderObject( m_renderObjects[3] );
    RO->getMesh()->colorize( Core::Colors::Red() );
    RO = roMgr->getRenderObject( m_renderObjects[4] );
    RO->getMesh()->colorize( Core::Colors::Green() );
    RO = roMgr->getRenderObject( m_renderObjects[5] );
    RO->getMesh()->colorize( Core::Colors::Blue() );
    // prepare selection
    int oldAxis = m_selectedAxis;
    int oldPlane = m_selectedPlane;
    m_selectedAxis = -1;
    m_selectedPlane = -1;
    if ( drawableIdx >= 0 )
    {
        auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(),
                                Core::Index( drawableIdx ) );
        if ( found != m_renderObjects.cend() )
        {
            int i = found - m_renderObjects.begin();
            if ( i < 3 )
            {
                m_selectedAxis = i;
                RO = roMgr->getRenderObject( m_renderObjects[m_selectedAxis] );
                RO->getMesh()->colorize( Core::Colors::Yellow() );
            } else
            {
                m_selectedPlane = i - 3;
                RO = roMgr->getRenderObject( m_renderObjects[m_selectedPlane + 3] );
                RO->getMesh()->colorize( Core::Colors::Yellow() );
                RO = roMgr->getRenderObject( m_renderObjects[( m_selectedPlane + 1 ) % 3] );
                RO->getMesh()->colorize( Core::Colors::Yellow() );
                RO = roMgr->getRenderObject( m_renderObjects[( m_selectedPlane + 2 ) % 3] );
                RO->getMesh()->colorize( Core::Colors::Yellow() );
            }
        }
    }
    if ( m_selectedAxis != oldAxis || m_selectedPlane != oldPlane )
    {
        m_initialPix = Core::Vector2::Zero();
        m_start = false;
    }
}

Core::Transform TranslateGizmo::mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY,
                                           bool stepped ) {
    static const Scalar step = 0.2;

    if ( m_selectedAxis == -1 && m_selectedPlane == -1 )
        return m_transform;

    // Get gizmo center and translation axis / plane normal
    std::vector<Scalar> hits;
    int axis = std::max( m_selectedAxis, m_selectedPlane );
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
    } else if ( m_selectedPlane > -1 )
    {
        found =
            findPointOnPlane( cam, origin, translateDir, m_initialPix + nextXY, endPoint, hits );
    }

    if ( found )
    {
        // Initialize translation
        if ( !m_start )
        {
            m_start = true;
            m_startPoint = endPoint;
            m_initialTrans = origin;
        }

        // Apply translation
        Ra::Core::Vector3 tr = endPoint - m_startPoint;
        if ( stepped )
        {
            tr = int( tr.norm() / step ) * step * tr.normalized();
        }
        m_transform.translation() = m_initialTrans + tr;
    }

    return m_transform;
}

void TranslateGizmo::setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY ) {
    const Core::Vector3 origin = m_transform.translation();
    const Core::Vector2 orgScreen = cam.project( origin );
    m_initialPix = orgScreen - initialXY;
}

} // namespace Gui
} // namespace Ra
