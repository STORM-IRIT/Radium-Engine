#include <GuiBase/Viewer/Gizmo/TranslateGizmo.hpp>

#include <Core/Container/VectorArray.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Math/RayCast.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>

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
TranslateGizmo::TranslateGizmo( Engine::Component* c, const Core::Math::Transform& worldTo,
                                const Core::Math::Transform& t, Mode mode ) :
    Gizmo( c, worldTo, t, mode ),
    m_startPoint( Core::Math::Vector3::Zero() ),
    m_initialPix( Core::Math::Vector2::Zero() ),
    m_selectedAxis( -1 ) {
    constexpr Scalar arrowScale = 0.2f;
    constexpr Scalar axisWidth = 0.05f;
    constexpr Scalar arrowFrac = 0.1f;

    // For x,y,z
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Math::Vector3 cylinderEnd = Core::Math::Vector3::Zero();
        Core::Math::Vector3 arrowEnd = Core::Math::Vector3::Zero();
        cylinderEnd[i] = ( 1.f - arrowFrac );
        arrowEnd[i] = 1.f;

        Core::Geometry::TriangleMesh cylinder = Core::Geometry::makeCylinder(
            Core::Math::Vector3::Zero(), arrowScale * cylinderEnd, arrowScale * axisWidth / 2.f );

        Core::Geometry::TriangleMesh cone = Core::Geometry::makeCone(
            arrowScale * cylinderEnd, arrowScale * arrowEnd, arrowScale * arrowFrac / 2.f );

        // Merge the cylinder and the cone to create the arrow shape.
        cylinder.append( cone );

        Core::Math::Color arrowColor = Core::Math::Color::Zero();
        arrowColor[i] = 1.f;
        Core::Container::Vector4Array colors( cylinder.m_vertices.size(), arrowColor );

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Arrow" ) );
        mesh->loadGeometry( cylinder );
        mesh->addData( Engine::Mesh::VERTEX_COLOR, colors );

        Engine::RenderObject* arrowDrawable =
            new Engine::RenderObject( "Gizmo Arrow", m_comp, Engine::RenderObjectType::UI );

        std::shared_ptr<Engine::RenderTechnique> rt( new Engine::RenderTechnique );
        rt->setConfiguration( Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );
        rt->resetMaterial( new Ra::Engine::BlinnPhongMaterial( "Default material" ) );
        arrowDrawable->setRenderTechnique( rt );
        arrowDrawable->setMesh( mesh );

        updateTransform( mode, m_worldTo, m_transform );

        m_renderObjects.push_back( m_comp->addRenderObject( arrowDrawable ) );
    }
}

void TranslateGizmo::updateTransform( Gizmo::Mode mode, const Core::Math::Transform& worldTo,
                                      const Core::Math::Transform& t ) {
    m_mode = mode;
    m_worldTo = worldTo;
    m_transform = t;
    Core::Math::Transform displayTransform = Core::Math::Transform::Identity();
    if ( m_mode == LOCAL )
    {
        displayTransform = m_transform;
    } else
    { displayTransform.translate( m_transform.translation() ); }

    for ( auto roIdx : m_renderObjects )
    {
        Engine::RadiumEngine::getInstance()
            ->getRenderObjectManager()
            ->getRenderObject( roIdx )
            ->setLocalTransform( m_worldTo * displayTransform );
    }
}

void TranslateGizmo::selectConstraint( int drawableIdx ) {
    int oldAxis = m_selectedAxis;
    m_selectedAxis = -1;
    if ( drawableIdx >= 0 )
    {
        auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(),
                                Core::Container::Index( drawableIdx ) );
        if ( found != m_renderObjects.cend() )
        {
            m_selectedAxis = int( found - m_renderObjects.begin() );
        }
    }
    if ( m_selectedAxis != oldAxis )
    {
        m_initialPix = Core::Math::Vector2::Zero();
        m_start = false;
    }
}

bool findPointOnAxis( const Engine::Camera& cam, const Ra::Core::Math::Vector3& origin,
                      const Ra::Core::Math::Vector3& axis, const Ra::Core::Math::Vector2& pix,
                      Ra::Core::Math::Vector3& pointOut ) {

    // Taken from Rodolphe's View engine gizmos -- see slide_axis().

    // Find a plane containing axis and as parallel as possible to
    // the camera image plane
    const Core::Math::Vector3 ortho = cam.getDirection().cross( axis );
    const Core::Math::Vector3 normal =
        ( ortho.squaredNorm() > 0 ) ? axis.cross( ortho ) : axis.cross( cam.getUpVector() );

    std::vector<Scalar> hit;
    const Core::Math::Ray ray = cam.getRayFromScreen( pix );
    bool hasHit = Core::Math::RayCast::vsPlane( ray, origin, normal, hit );
    if ( hasHit )
    {
        pointOut = origin + ( axis.dot( ray.pointAt( hit[0] ) - origin ) ) * axis;
    }
    return hasHit;
}

Core::Math::Transform TranslateGizmo::mouseMove( const Engine::Camera& cam, const Core::Math::Vector2& nextXY,
                                           bool stepped ) {
    static const float step = 0.2;
    if ( m_selectedAxis >= 0 )
    {
        const Core::Math::Vector3 origin = m_transform.translation();
        Core::Math::Vector3 translateDir =
            m_mode == LOCAL
                ? Core::Math::Vector3( m_transform.rotation() * Core::Math::Vector3::Unit( m_selectedAxis ) )
                : Core::Math::Vector3::Unit( m_selectedAxis );

        if ( !m_start )
        {
            if ( findPointOnAxis( cam, origin, translateDir, m_initialPix + nextXY, m_startPoint ) )
            {
                m_start = true;
                m_initialTrans = m_transform.translation();
            }
        }

        Ra::Core::Math::Vector3 endPoint;
        if ( findPointOnAxis( cam, origin, translateDir, m_initialPix + nextXY, endPoint ) )
        {
            if ( stepped )
            {
                const Ra::Core::Math::Vector3 tr = endPoint - m_startPoint;
                m_transform.translation() =
                    m_initialTrans + int( tr.norm() / step ) * step * tr.normalized();
            } else
            { m_transform.translation() = m_initialTrans + endPoint - m_startPoint; }
        }
    }
    return m_transform;
}

void TranslateGizmo::setInitialState( const Engine::Camera& cam, const Core::Math::Vector2& initialXY ) {
    const Core::Math::Vector3 origin = m_transform.translation();
    const Core::Math::Vector2 orgScreen = cam.project( origin );
    m_initialPix = orgScreen - initialXY;
}
} // namespace Gui
} // namespace Ra
