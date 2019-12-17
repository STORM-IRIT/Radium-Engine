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

RotateGizmo::RotateGizmo( Engine::Component* c,
                          const Core::Transform& worldTo,
                          const Core::Transform& t,
                          Mode mode ) :
    Gizmo( c, worldTo, t, mode ),
    m_initialPix( Core::Vector2::Zero() ),
    m_selectedAxis( -1 ) {
    constexpr Scalar torusOutRadius   = .1_ra;
    constexpr Scalar torusAspectRatio = .08_ra;
    // For x,y,z
    for ( uint i = 0; i < 3; ++i )
    {
        Core::Geometry::TriangleMesh torus = Core::Geometry::makeParametricTorus<32>(
            torusOutRadius, torusAspectRatio * torusOutRadius );
        // Transform the torus from z-axis to axis i.

        auto& data = torus.verticesWithLock();
        for ( auto& v : data )
        {
            v = .5_ra * v;
            if ( i < 2 ) { std::swap( v[2], v[i] ); }
        }
        torus.verticesUnlock();

        // set color
        {
            Core::Utils::Color color = Core::Utils::Color::Black();
            color[i]                 = 1_ra;
            auto colorAttribHandle   = torus.addAttrib<Core::Vector4>( colorAttribName );
            torus.getAttrib( colorAttribHandle )
                .setData( Core::Vector4Array( torus.vertices().size(), color ) );
        }

        auto mesh = std::shared_ptr<Engine::Mesh>( new Engine::Mesh( "Gizmo Torus" ) );
        mesh->loadGeometry( std::move( torus ) );

        Engine::RenderObject* arrowDrawable =
            new Engine::RenderObject( "Gizmo Torus", m_comp, Engine::RenderObjectType::UI );

        std::shared_ptr<Engine::RenderTechnique> rt( new Engine::RenderTechnique );
        rt->setConfiguration( Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );
        rt->resetMaterial( new Ra::Engine::BlinnPhongMaterial( "Default material" ) );
        arrowDrawable->setRenderTechnique( rt );
        arrowDrawable->setMesh( mesh );

        updateTransform( mode, m_worldTo, m_transform );

        addRenderObject( arrowDrawable, mesh );
    }
}

void RotateGizmo::updateTransform( Gizmo::Mode mode,
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

    /// \fixme Cause multiple search in Ro map.
    for ( auto roIdx : roIds() )
    {
        Engine::RadiumEngine::getInstance()
            ->getRenderObjectManager()
            ->getRenderObject( roIdx )
            ->setLocalTransform( m_worldTo * displayTransform );
    }
}

void RotateGizmo::selectConstraint( int drawableIdx ) {

    // reColor constraint
    if ( m_selectedAxis != -1 )
    {
        Core::Utils::Color color = Core::Utils::Color::Black();
        color[m_selectedAxis]    = 1_ra;
        const auto& mesh         = roMeshes()[size_t( m_selectedAxis )];
        mesh->getCoreGeometry().colorize( color );
    }
    // prepare selection
    m_selectedAxis = -1;
    if ( drawableIdx >= 0 )
    {
        auto found =
            std::find( roIds().cbegin(), roIds().cend(), Core::Utils::Index( drawableIdx ) );
        if ( found != roIds().cend() )
        {
            m_selectedAxis   = int( std::distance( roIds().cbegin(), found ) );
            const auto& mesh = roMeshes()[size_t( m_selectedAxis )];
            mesh->getCoreGeometry().colorize( Core::Utils::Color::Yellow() );
        }
    }
}

Core::Transform RotateGizmo::mouseMove( const Engine::Camera& cam,
                                        const Core::Vector2& nextXY,
                                        bool stepped,
                                        bool /*whole*/ ) {
    static const Scalar step = Ra::Core::Math::Pi / 10_ra;

    if ( m_selectedAxis == -1 ) return m_transform;

    // Decompose the current transform's linear part into rotation and scale
    Core::Matrix3 rotationMat;
    Core::Matrix3 scaleMat;
    m_transform.computeRotationScaling( &rotationMat, &scaleMat );

    // Get gizmo center and rotation axis
    const Core::Vector3 origin = m_transform.translation();
    Core::Vector3 rotationAxis = Core::Vector3::Unit( m_selectedAxis );
    if ( m_mode == LOCAL ) { rotationAxis = rotationMat * rotationAxis; }
    rotationAxis.normalize();
    const Core::Vector3 originW       = m_worldTo * origin;
    const Core::Vector3 rotationAxisW = m_worldTo * rotationAxis;

    // Initialize rotation
    if ( !m_start )
    {
        m_start      = true;
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
    if ( hit1 && hit2 && hits1[0] > .2_ra && hits2[0] > .2_ra )
    {
        // Do the calculations relative to the circle center.
        originalHit -= originW;
        currentHit -= originW;

        // Get the angle between the two vectors with the correct sign
        // (since we already know our current rotation axis).
        auto c   = originalHit.cross( currentHit );
        Scalar d = originalHit.dot( currentHit );

        angle = Core::Math::sign( c.dot( rotationAxisW ) ) * std::atan2( c.norm(), d );
    }
    else
    {
        // Rotation plane is orthogonal to the image plane
        Core::Vector2 dir =
            ( cam.project( originW + rotationAxisW ) - cam.project( originW ) ).normalized();
        if ( std::abs( dir( 0 ) ) < 1e-3_ra ) { dir << 1, 0; }
        else if ( std::abs( dir( 1 ) ) < 1e-3_ra )
        { dir << 0, 1; }
        else
        { dir = Core::Vector2( dir( 1 ), -dir( 0 ) ); }
        Scalar diag = std::min( cam.getWidth(), cam.getHeight() );
        angle       = dir.dot( ( nextXY - m_initialPix ) ) * 8_ra / diag;
    }
    if ( std::isnan( angle ) ) { angle = 0_ra; }
    // Apply rotation
    Core::Vector2 nextXY_ = nextXY;
    if ( stepped )
    {
        angle = int( angle / step ) * step;
        if ( Core::Math::areApproxEqual( angle, 0_ra ) ) { nextXY_ = m_initialPix; }
        if ( !m_stepped )
        {
            Scalar diff = m_totalAngle - int( m_totalAngle / step ) * step;
            angle -= diff;
        }
    }
    m_stepped = stepped;
    m_totalAngle += angle;
    if ( !Core::Math::areApproxEqual( angle, 0_ra ) )
    {
        auto newRot = Core::AngleAxis( angle, rotationAxis ) * rotationMat;
        m_transform.fromPositionOrientationScale( origin, newRot, scaleMat.diagonal() );
    }
    m_initialPix = nextXY_;

    return m_transform;
}

void RotateGizmo::setInitialState( const Engine::Camera& /*cam*/, const Core::Vector2& initialXY ) {
    m_initialPix = initialXY;
    m_start      = false;
    m_stepped    = false;
}

} // namespace Gui
} // namespace Ra
