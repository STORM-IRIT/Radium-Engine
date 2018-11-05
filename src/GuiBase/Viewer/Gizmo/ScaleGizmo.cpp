#include <GuiBase/Viewer/Gizmo/ScaleGizmo.hpp>

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

#include <GuiBase/Utils/Keyboard.hpp>

namespace Ra {
namespace Gui {

inline void colorMesh( std::shared_ptr<Engine::Mesh> mesh, const Core::Color& color ) {
    Core::Vector4Array colors( mesh->getGeometry().vertices().size(), color );
    mesh->addData( Engine::Mesh::VERTEX_COLOR, colors );
}

ScaleGizmo::ScaleGizmo( Engine::Component* c, const Core::Transform& worldTo,
                        const Core::Transform& t, Mode mode ) :
    Gizmo( c, worldTo, t, mode ),
    m_prevScale( Core::Vector3::Ones() ),
    m_startPoint( Core::Vector3::Zero() ),
    m_initialPix( Core::Vector2::Zero() ),
    m_selectedAxis( -1 ),
    m_selectedPlane( -1 ) {
    constexpr Scalar arrowScale = 0.1f;
    constexpr Scalar axisWidth = 0.05f;
    constexpr Scalar arrowFrac = 0.125f;
    constexpr Scalar radius = arrowScale * axisWidth / 2.f;

    std::shared_ptr<Engine::RenderTechnique> rt( new Engine::RenderTechnique );
    rt->setConfiguration( Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );
    rt->resetMaterial( new Engine::BlinnPhongMaterial( "Default material" ) );

    // For x,y,z
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Vector3 cylinderEnd = Core::Vector3::Zero();
        Core::Vector3 arrowEnd = Core::Vector3::Zero();
        cylinderEnd[i] = ( 1.f - arrowFrac );
        arrowEnd[i] = 1.f;

        Core::TriangleMesh cylinder = Core::MeshUtils::makeCylinder(
            Core::Vector3::Zero(), arrowScale * cylinderEnd, radius );

        Core::Aabb box;
        box.extend( Core::Vector3( -radius * 2, -radius * 2, -radius * 2 ) );
        box.extend( Core::Vector3( radius * 2, radius * 2, radius * 2 ) );
        box.translate( arrowScale * cylinderEnd );
        Core::TriangleMesh cone = Core::MeshUtils::makeSharpBox( box );

        // Merge the cylinder and the cone to create the arrow shape.
        cylinder.append( cone );

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Arrow" ) );
        mesh->loadGeometry( cylinder );
        Core::Color arrowColor = Core::Color::Zero();
        arrowColor[i] = 1.f;
        colorMesh( mesh, arrowColor );

        Engine::RenderObject* arrowDrawable =
            new Engine::RenderObject( "Gizmo Arrow", m_comp, Engine::RenderObjectType::UI );
        arrowDrawable->setRenderTechnique( rt );
        arrowDrawable->setMesh( mesh );

        m_renderObjects.push_back( m_comp->addRenderObject( arrowDrawable ) );
    }

    // For xy,yz,zx
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Vector3 axis = Core::Vector3::Zero();
        axis[( i == 0 ? 1 : ( i == 1 ? 0 : 2 ) )] = 1;
        Core::Transform T = Core::Transform::Identity();
        T.rotate( Core::AngleAxis( Core::Math::PiDiv2, axis ) );
        T.translation()[( i + 1 ) % 3] += arrowFrac * arrowScale;
        T.translation()[( i + 2 ) % 3] += arrowFrac * arrowScale;

        Core::TriangleMesh plane = Core::MeshUtils::makePlaneGrid(
            1, 1, Core::Vector2( arrowFrac * arrowScale, arrowFrac * arrowScale ), T );
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
        colorMesh( mesh, planeColor );

        Engine::RenderObject* planeDrawable =
            new Engine::RenderObject( "Gizmo Plane", m_comp, Engine::RenderObjectType::UI );
        planeDrawable->setRenderTechnique( rt );
        planeDrawable->setMesh( mesh );
        m_renderObjects.push_back( m_comp->addRenderObject( planeDrawable ) );
    }

    // update all
    updateTransform( mode, m_worldTo, m_transform );
}

void ScaleGizmo::updateTransform( Gizmo::Mode mode, const Core::Transform& worldTo,
                                  const Core::Transform& t ) {
    m_mode = mode;
    m_worldTo = worldTo;
    m_transform = t;
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

    for ( auto roIdx : m_renderObjects )
    {
        Engine::RadiumEngine::getInstance()
            ->getRenderObjectManager()
            ->getRenderObject( roIdx )
            ->setLocalTransform( m_worldTo * displayTransform );
    }
}

void ScaleGizmo::selectConstraint( int drawableIdx ) {
    // reColor constraints
    auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();
    auto RO = roMgr->getRenderObject( m_renderObjects[0] );
    colorMesh( RO->getMesh(), Core::Colors::Red() );
    RO = roMgr->getRenderObject( m_renderObjects[1] );
    colorMesh( RO->getMesh(), Core::Colors::Green() );
    RO = roMgr->getRenderObject( m_renderObjects[2] );
    colorMesh( RO->getMesh(), Core::Colors::Blue() );
    RO = roMgr->getRenderObject( m_renderObjects[3] );
    colorMesh( RO->getMesh(), Core::Colors::Red() );
    RO = roMgr->getRenderObject( m_renderObjects[4] );
    colorMesh( RO->getMesh(), Core::Colors::Green() );
    RO = roMgr->getRenderObject( m_renderObjects[5] );
    colorMesh( RO->getMesh(), Core::Colors::Blue() );
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
                auto RO = roMgr->getRenderObject( m_renderObjects[m_selectedAxis] );
                colorMesh( RO->getMesh(), Core::Colors::Yellow() );
            } else if ( i < 6 )
            {
                m_selectedPlane = i - 3;
                auto RO = roMgr->getRenderObject( m_renderObjects[m_selectedPlane + 3] );
                colorMesh( RO->getMesh(), Core::Colors::Yellow() );
            }
        }
    }
    if ( m_selectedAxis != oldAxis || m_selectedPlane != oldPlane )
    {
        m_initialPix = Core::Vector2::Zero();
        m_start = false;
    }
}

inline bool findPointOnAxis( const Engine::Camera& cam, const Core::Vector3& origin,
                             const Core::Vector3& axis, const Core::Vector2& pix,
                             Core::Vector3& pointOut ) {

    // Taken from Rodolphe's View engine gizmos -- see slide_axis().

    // Find a plane containing axis and as parallel as possible to
    // the camera image plane
    const Core::Vector3 ortho = cam.getDirection().cross( axis );
    const Core::Vector3 normal =
        ( ortho.squaredNorm() > 0 ) ? axis.cross( ortho ) : axis.cross( cam.getUpVector() );

    std::vector<Scalar> hit;
    const Core::Ray ray = cam.getRayFromScreen( pix );
    bool hasHit = Core::RayCast::vsPlane( ray, origin, normal, hit );
    if ( hasHit )
    {
        pointOut = origin + ( axis.dot( ray.pointAt( hit[0] ) - origin ) ) * axis;
    }
    return hasHit;
}

inline bool findPointOnPlane( const Engine::Camera& cam, const Core::Vector3& origin,
                              const Core::Vector3& axis, const Core::Vector2& pix,
                              Core::Vector3& pointOut ) {

    // Taken from Rodolphe's View engine gizmos -- see slide_plane().

    std::vector<Scalar> hit;
    const Core::Ray ray = cam.getRayFromScreen( pix );
    bool hasHit = Core::RayCast::vsPlane( ray, origin, axis, hit );
    if ( hasHit )
    {
        pointOut = ray.pointAt( hit[0] );
    }
    return hasHit;
}

Core::Transform ScaleGizmo::mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY,
                                       bool stepped ) {
    static const Scalar step = 0.2;

    bool whole = isKeyPressed( 0x01000020 ); // shift 16777248
    auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();
    if ( whole )
    {
        auto RO = roMgr->getRenderObject( m_renderObjects[0] );
        colorMesh( RO->getMesh(), Core::Colors::Yellow() );
        RO = roMgr->getRenderObject( m_renderObjects[1] );
        colorMesh( RO->getMesh(), Core::Colors::Yellow() );
        RO = roMgr->getRenderObject( m_renderObjects[2] );
        colorMesh( RO->getMesh(), Core::Colors::Yellow() );
        RO = roMgr->getRenderObject( m_renderObjects[3] );
        colorMesh( RO->getMesh(), Core::Colors::Yellow() );
        RO = roMgr->getRenderObject( m_renderObjects[4] );
        colorMesh( RO->getMesh(), Core::Colors::Yellow() );
        RO = roMgr->getRenderObject( m_renderObjects[5] );
        colorMesh( RO->getMesh(), Core::Colors::Yellow() );
    } else
    {
        auto RO = roMgr->getRenderObject( m_renderObjects[0] );
        colorMesh( RO->getMesh(), Core::Colors::Red() );
        RO = roMgr->getRenderObject( m_renderObjects[1] );
        colorMesh( RO->getMesh(), Core::Colors::Green() );
        RO = roMgr->getRenderObject( m_renderObjects[2] );
        colorMesh( RO->getMesh(), Core::Colors::Blue() );
        RO = roMgr->getRenderObject( m_renderObjects[3] );
        colorMesh( RO->getMesh(), Core::Colors::Red() );
        RO = roMgr->getRenderObject( m_renderObjects[4] );
        colorMesh( RO->getMesh(), Core::Colors::Green() );
        RO = roMgr->getRenderObject( m_renderObjects[5] );
        colorMesh( RO->getMesh(), Core::Colors::Blue() );
        if ( m_selectedAxis > 0 )
        {
            RO = roMgr->getRenderObject( m_renderObjects[m_selectedAxis] );
            colorMesh( RO->getMesh(), Core::Colors::Yellow() );
        }
        if ( m_selectedPlane > 0 )
        {
            RO = roMgr->getRenderObject( m_renderObjects[m_selectedPlane + 3] );
            colorMesh( RO->getMesh(), Core::Colors::Yellow() );
        }
    }

    if ( m_selectedAxis == -1 && m_selectedPlane == -1 && !whole )
    {
        return m_transform;
    }

    if ( whole )
    {
        const Core::Vector3 origin = m_transform.translation();
        Core::Vector3 scaleDir = Core::Vector3( m_transform.rotation() * Core::Vector3::Unit( 0 ) );

        if ( !m_start )
        {
            if ( findPointOnPlane( cam, origin, scaleDir, m_initialPix + nextXY, m_startPoint ) )
            {
                m_start = true;
                m_startPos = m_transform.translation();
                m_prevScale = Core::Vector3::Ones();
            }
        }

        Core::Vector3 endPoint;
        if ( findPointOnPlane( cam, origin, scaleDir, m_initialPix + nextXY, endPoint ) )
        {
            const Core::Vector3 a = endPoint - m_startPos;
            if ( a.squaredNorm() < 1e-3 )
            {
                return m_transform;
            }
            const Core::Vector3 b = m_startPoint - m_startPos;
            Core::Vector3 scale = a.norm() / b.norm() * Core::Vector3::Ones();
            m_transform.scale( m_prevScale );
            if ( stepped )
            {
                scale = int( ( a.norm() / b.norm() ) / step + 1 ) * step * Core::Vector3::Ones();
            }
            m_prevScale = scale;
            m_transform.scale( m_prevScale );
            m_prevScale = m_prevScale.cwiseInverse();
        }
    } else if ( m_selectedAxis >= 0 )
    {
        const Core::Vector3 origin = m_transform.translation();
        const Core::Vector3 scaleDir =
            Core::Vector3( m_transform.rotation() * Core::Vector3::Unit( m_selectedAxis ) );
        if ( !m_start )
        {
            if ( findPointOnAxis( cam, origin, scaleDir, m_initialPix + nextXY, m_startPoint ) )
            {
                m_start = true;
                m_startPos = m_transform.translation();
                m_prevScale = Core::Vector3::Ones();
            }
        }

        Core::Vector3 endPoint;
        if ( findPointOnAxis( cam, origin, scaleDir, m_initialPix + nextXY, endPoint ) )
        {
            const Core::Vector3 a = endPoint - m_startPos;
            if ( a.squaredNorm() < 1e-3 )
            {
                return m_transform;
            }
            const Core::Vector3 b = m_startPoint - m_startPos;
            Core::Vector3 scale = Core::Vector3::Ones();
            m_transform.scale( m_prevScale );
            if ( stepped )
            {
                scale[m_selectedAxis] = int( ( a.norm() / b.norm() ) / step + 1 ) * step;
            } else
            { scale[m_selectedAxis] = a.norm() / b.norm(); }
            m_prevScale = scale;
            m_transform.scale( m_prevScale );
            m_prevScale = m_prevScale.cwiseInverse();
        }
    } else if ( m_selectedPlane >= 0 )
    {
        const Core::Vector3 origin = m_transform.translation();
        Core::Vector3 scaleDir =
            Core::Vector3( m_transform.rotation() * Core::Vector3::Unit( m_selectedPlane ) );

        if ( !m_start )
        {
            if ( findPointOnPlane( cam, origin, scaleDir, m_initialPix + nextXY, m_startPoint ) )
            {
                m_start = true;
                m_startPos = m_transform.translation();
                m_prevScale = Core::Vector3::Ones();
            }
        }

        Core::Vector3 endPoint;
        if ( findPointOnPlane( cam, origin, scaleDir, m_initialPix + nextXY, endPoint ) )
        {
            const Core::Vector3 a = endPoint - m_startPos;
            if ( a.squaredNorm() < 1e-3 )
            {
                return m_transform;
            }
            const Core::Vector3 b = m_startPoint - m_startPos;
            Core::Vector3 scale = Core::Vector3::Ones();
            m_transform.scale( m_prevScale );
            if ( stepped )
            {
                scale[( m_selectedPlane + 1 ) % 3] =
                    int( ( a.norm() / b.norm() ) / step + 1 ) * step;
                scale[( m_selectedPlane + 2 ) % 3] =
                    int( ( a.norm() / b.norm() ) / step + 1 ) * step;
            } else
            {
                scale[( m_selectedPlane + 1 ) % 3] = a.norm() / b.norm();
                scale[( m_selectedPlane + 2 ) % 3] = a.norm() / b.norm();
            }
            m_prevScale = scale;
            m_transform.scale( m_prevScale );
            m_prevScale = m_prevScale.cwiseInverse();
        }
    }
    return m_transform;
}

void ScaleGizmo::setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY ) {
    const Core::Vector3 origin = m_transform.translation();
    const Core::Vector2 orgScreen = cam.project( origin );
    m_initialPix = orgScreen - initialXY;
}

} // namespace Gui
} // namespace Ra
