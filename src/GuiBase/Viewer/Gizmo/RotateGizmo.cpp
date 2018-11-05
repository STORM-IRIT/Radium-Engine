#include <GuiBase/Viewer/Gizmo/RotateGizmo.hpp>

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

inline void colorMesh( std::shared_ptr<Engine::Mesh> mesh, const Core::Color& color ) {
    Core::Vector4Array colors( mesh->getGeometry().vertices().size(), color );
    mesh->addData( Engine::Mesh::VERTEX_COLOR, colors );
}

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
        Core::TriangleMesh torus = Core::MeshUtils::makeParametricTorus<32>(
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

        std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh( "Gizmo Arrow" ) );
        mesh->loadGeometry( torus );
        Core::Color color = Core::Color::Zero();
        color[i] = 1.f;
        colorMesh( mesh, color );

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
    // reColor constraint
    if ( m_selectedAxis != -1 )
    {
        Core::Color color = Core::Color::Zero();
        color[m_selectedAxis] = 1.f;
        auto RO = roMgr->getRenderObject( m_renderObjects[m_selectedAxis] );
        colorMesh( RO->getMesh(), color );
    }
    // prepare selection
    int oldAxis = m_selectedAxis;
    m_selectedAxis = -1;
    if ( drawableIdx >= 0 )
    {
        auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(),
                                Core::Index( drawableIdx ) );
        if ( found != m_renderObjects.cend() )
        {
            m_selectedAxis = int( found - m_renderObjects.begin() );
            auto RO = roMgr->getRenderObject( m_renderObjects[m_selectedAxis] );
            colorMesh( RO->getMesh(), Core::Colors::Yellow() );
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
    if ( m_selectedAxis >= 0 )
    {
        const Core::Vector3 origin = m_transform.translation();
        Core::Vector3 rotationAxis = Core::Vector3::Unit( m_selectedAxis );

        // Decompose the current transform's linear part into rotation and scale
        Core::Matrix3 rotationMat;
        Core::Matrix3 scaleMat;
        m_transform.computeRotationScaling( &rotationMat, &scaleMat );

        if ( m_mode == LOCAL )
        {
            rotationAxis = rotationMat * rotationAxis;
        }
        rotationAxis.normalize();

        if ( !m_start )
        {
            m_start = true;
            m_totalAngle = 0;
            m_initialRot = rotationMat;
        }

        // Project the clicked points against the plane defined by the rotation circles.
        std::vector<Scalar> hits1, hits2;
        Core::Ray rayToFirstClick = cam.getRayFromScreen( m_initialPix );
        Core::Ray rayToCurrentClick = cam.getRayFromScreen( nextXY );
        bool hit1 = Core::RayCast::vsPlane( rayToFirstClick, m_worldTo * origin,
                                            m_worldTo * rotationAxis, hits1 );
        bool hit2 = Core::RayCast::vsPlane( rayToCurrentClick, m_worldTo * origin,
                                            m_worldTo * rotationAxis, hits2 );

        Core::Vector2 nextXY_ = nextXY;
        if ( hit1 && hit2 )
        {
            // Do the calculations relative to the circle center.
            const Core::Vector3 originalHit =
                rayToFirstClick.pointAt( hits1[0] ) - m_worldTo * origin;
            const Core::Vector3 currentHit =
                rayToCurrentClick.pointAt( hits2[0] ) - m_worldTo * origin;

            // Get the angle between the two vectors with the correct sign
            // (since we already know our current rotation axis).
            auto c = originalHit.cross( currentHit );
            Scalar d = originalHit.dot( currentHit );

            Scalar angle =
                Core::Math::sign( c.dot( m_worldTo * rotationAxis ) ) * std::atan2( c.norm(), d );

            // Apply rotation.
            if ( stepped )
            {
                angle = int( angle / step ) * step;
                if ( angle == 0 )
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
            if ( angle != 0 )
            {
                auto newRot = Core::AngleAxis( angle, rotationAxis ) * rotationMat;
                m_transform.fromPositionOrientationScale( origin, newRot, scaleMat.diagonal() );
            }
        }
        m_initialPix = nextXY_;
    }
    return m_transform;
}

void RotateGizmo::setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY ) {
    m_initialPix = initialXY;
}
} // namespace Gui
} // namespace Ra
