#include <GuiBase/Viewer/Gizmo/RotateGizmo.hpp>

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

RotateGizmo::RotateGizmo( Engine::Component* c, const Core::Transform& worldTo,
                          const Core::Transform& t, Mode mode ) :
    Gizmo( c, worldTo, t, mode ),
    m_initialPix( Core::Vector2::Zero() ),
    m_selectedAxis( -1 ) {
    constexpr Scalar torusOutRadius = 0.1f;
    constexpr Scalar torusAspectRatio = 0.08f;
    // For x,y,z
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Geometry::TriangleMesh torus = Core::Geometry::makeParametricTorus<32>(
            torusOutRadius, torusAspectRatio * torusOutRadius );
        // Transform the torus from z-axis to axis i.
        for ( auto& v : torus.vertices() )
        {
            v = 0.5f * v;
            if ( i < 2 )
            {
                std::swap( v[2], v[i] );
            }
        }

        // set color
        {
            Core::Utils::Color color = Core::Utils::Color::Black();
            color[i] = 1.f;
            auto colorAttribHandle = torus.addAttrib<Core::Vector4>( colorAttribName );
            auto colorAttrib = torus.getAttrib( colorAttribHandle ).data() =
                Core::Vector4Array( torus.vertices().size(), color );
        }

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Arrow" ) );
        mesh->loadGeometry( std::move( torus ) );

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

void RotateGizmo::updateTransform( Gizmo::Mode mode, const Core::Transform& worldTo,
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

void RotateGizmo::selectConstraint( int drawableIdx ) {
    auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();

    auto colorizeMesh = [roMgr]( int id, const Core::Utils::Color& color ) {
        auto rendermesh = roMgr->getRenderObject( id )->getMesh();
        CORE_ASSERT( rendermesh != nullptr, "Cannot access Gizmo render mesh" );

        // \warning: this is ugly and might generate a std::bad cast.
        // An alternative implementation would be to store references to the gizmo meshes and use
        // them instead of using the roMgr.
        Core::Geometry::TriangleMesh& mesh =
            dynamic_cast<Core::Geometry::TriangleMesh&>( rendermesh->getGeometry() );
        auto colorAttribHandle = mesh.getAttribHandle<Core::Vector4>( colorAttribName );
        CORE_ASSERT( mesh.isValid( colorAttribHandle ), "Gizmo mesh should have colors" );
        auto colorAttrib = mesh.getAttrib( colorAttribHandle ).data() =
            Core::Vector4Array( mesh.vertices().size(), color );
        rendermesh->setDirty( Engine::Mesh::VERTEX_COLOR );
    };

    // reColor constraint
    if ( m_selectedAxis != -1 )
    {
        Core::Utils::Color color = Core::Utils::Color::Black();
        color[m_selectedAxis] = 1.f;
        colorizeMesh( m_renderObjects[m_selectedAxis], color );
    }
    // prepare selection
    int oldAxis = m_selectedAxis;
    m_selectedAxis = -1;
    if ( drawableIdx >= 0 )
    {
        auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(),
                                Core::Utils::Index( drawableIdx ) );
        if ( found != m_renderObjects.cend() )
        {
            m_selectedAxis = int( found - m_renderObjects.begin() );
            colorizeMesh( m_renderObjects[m_selectedAxis], Core::Utils::Color::Yellow() );
        }
    }
    if ( m_selectedAxis != oldAxis )
    {
        m_start = false;
        m_stepped = false;
    }
}

Core::Transform RotateGizmo::mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY,
                                        bool stepped ) {
    static const Scalar step = Ra::Core::Math::Pi / 10.f;

    if ( m_selectedAxis == -1 )
        return m_transform;

    // Decompose the current transform's linear part into rotation and scale
    Core::Matrix3 rotationMat;
    Core::Matrix3 scaleMat;
    m_transform.computeRotationScaling( &rotationMat, &scaleMat );

    // Get gizmo center and rotation axis
    const Core::Vector3 origin = m_transform.translation();
    Core::Vector3 rotationAxis = Core::Vector3::Unit( m_selectedAxis );
    if ( m_mode == LOCAL )
    {
        rotationAxis = rotationMat * rotationAxis;
    }
    rotationAxis.normalize();
    const Core::Vector3 originW = m_worldTo * origin;
    const Core::Vector3 rotationAxisW = m_worldTo * rotationAxis;

    // Initialize rotation
    if ( !m_start )
    {
        m_start = true;
        m_totalAngle = 0;
        m_initialRot = rotationMat;
    }

    // Project the clicked points against the plane defined by the rotation circles.
    std::vector<Scalar> hits1, hits2;
    Core::Vector3 originalHit, currentHit;
    bool hit1 = findPointOnPlane( cam, originW, rotationAxisW, m_initialPix, originalHit, hits1 );
    bool hit2 = findPointOnPlane( cam, originW, rotationAxisW, nextXY, currentHit, hits2 );

    // Compute the rotation angle
    Scalar angle;
    // standard check  +  guard against precision issues
    if ( hit1 && hit2 && hits1[0] > 0.2 && hits2[0] > 0.2 )
    {
        // Do the calculations relative to the circle center.
        originalHit -= originW;
        currentHit -= originW;

        // Get the angle between the two vectors with the correct sign
        // (since we already know our current rotation axis).
        auto c = originalHit.cross( currentHit );
        Scalar d = originalHit.dot( currentHit );

        angle = Core::Math::sign( c.dot( rotationAxisW ) ) * std::atan2( c.norm(), d );
    } else
    {
        // Rotation plane is orthogonal to the image plane
        Core::Vector2 dir =
            ( cam.project( originW + rotationAxisW ) - cam.project( originW ) ).normalized();
        if ( std::abs( dir( 0 ) ) < 1e-3 )
        {
            dir << 1, 0;
        } else if ( std::abs( dir( 1 ) ) < 1e-3 )
        {
            dir << 0, 1;
        } else
        { dir = Core::Vector2( dir( 1 ), -dir( 0 ) ); }
        Scalar diag = std::min( cam.getWidth(), cam.getHeight() );
        angle = dir.dot( ( nextXY - m_initialPix ) ) * 8 / diag;
    }
    if ( std::isnan( angle ) )
    {
        angle = Scalar( 0 );
    }
    // Apply rotation
    Core::Vector2 nextXY_ = nextXY;
    if ( stepped )
    {
        angle = int( angle / step ) * step;
        if ( Core::Math::areApproxEqual( angle, 0_ra ) )
        {
            nextXY_ = m_initialPix;
        }
        if ( !m_stepped )
        {
            Scalar diff = m_totalAngle - int( m_totalAngle / step ) * step;
            angle -= diff;
        }
    }
    m_stepped = stepped;
    m_totalAngle += angle;
    if ( ! Core::Math::areApproxEqual( angle, 0_ra ) )
    {
        auto newRot = Core::AngleAxis( angle, rotationAxis ) * rotationMat;
        m_transform.fromPositionOrientationScale( origin, newRot, scaleMat.diagonal() );
    }
    m_initialPix = nextXY_;

    return m_transform;
}

void RotateGizmo::setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY ) {
    m_initialPix = initialXY;
}

} // namespace Gui
} // namespace Ra
