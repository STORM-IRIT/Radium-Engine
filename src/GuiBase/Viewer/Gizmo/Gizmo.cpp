#include <GuiBase/Viewer/Gizmo/Gizmo.hpp>

#include <Core/Geometry/RayCast.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Engine/Renderer/Material/PlainMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

namespace Ra {
namespace Gui {

std::array<std::shared_ptr<Ra::Engine::Material>, 4> Gizmo::s_material;

Gizmo::Gizmo( Engine::Component* c,
              const Core::Transform& worldTo,
              const Core::Transform& t,
              Mode mode ) :
    m_worldTo( worldTo ),
    m_transform( t ),
    m_comp( c ),
    m_mode( mode ) {
    using namespace Core::Utils;
    if ( !s_material[0] )
    {
        auto mat      = Core::make_shared<Engine::PlainMaterial>( "GizmoRedMaterial" );
        mat->m_color  = Color::Red();
        s_material[0] = mat;
        mat           = Core::make_shared<Engine::PlainMaterial>( "GizmoGreenMaterial" );
        mat->m_color  = Color::Green();
        s_material[1] = mat;
        mat           = Core::make_shared<Engine::PlainMaterial>( "GizmoBlueMaterial" );
        mat->m_color  = Color::Blue();
        s_material[2] = mat;
        mat           = Core::make_shared<Engine::PlainMaterial>( "GizmoYellowMaterial" );
        mat->m_color  = Color::Yellow();
        s_material[3] = mat;
    }
}

Gizmo::~Gizmo() {
    // first release meshes shared pointers before destroying ROs
    for ( auto ro : m_ros )
    {
        m_comp->removeRenderObject( ro->getIndex() );
    }
    m_ros.clear();
}

void Gizmo::show( bool on ) {
    for ( auto ro : m_ros )
    {
        ro->setVisible( on );
    }
}

bool Gizmo::findPointOnAxis( const Engine::Camera& cam,
                             const Core::Vector3& origin,
                             const Core::Vector3& axis,
                             const Core::Vector2& pix,
                             Core::Vector3& pointOut,
                             std::vector<Scalar>& hits ) {
    // Taken from Rodolphe's View engine gizmos -- see slide_axis().
    // Find a plane containing axis and as parallel as possible to
    // the camera image plane
    const Core::Vector3 ortho = cam.getDirection().cross( axis );
    const Core::Vector3 normal =
        ( ortho.squaredNorm() > 0 ) ? axis.cross( ortho ) : axis.cross( cam.getUpVector() );

    const auto ray = cam.getRayFromScreen( pix );
    bool hasHit    = Core::Geometry::RayCastPlane( ray, origin, normal, hits );
    if ( hasHit ) { pointOut = origin + ( axis.dot( ray.pointAt( hits[0] ) - origin ) ) * axis; }
    return hasHit;
}

bool Gizmo::findPointOnPlane( const Engine::Camera& cam,
                              const Core::Vector3& origin,
                              const Core::Vector3& axis,
                              const Core::Vector2& pix,
                              Core::Vector3& pointOut,
                              std::vector<Scalar>& hits ) {
    // Taken from Rodolphe's View engine gizmos -- see slide_plane().
    const auto ray = cam.getRayFromScreen( pix );
    bool hasHit    = Core::Geometry::RayCastPlane( ray, origin, axis, hits );
    if ( hasHit ) { pointOut = ray.pointAt( hits[0] ); }
    return hasHit;
}

void Gizmo::addRenderObject( Engine::RenderObject* ro ) {
    m_comp->addRenderObject( ro );
    m_ros.push_back( ro );
}

void Gizmo::changeMat( uint ro, uint mat ) {
    m_ros[ro]->setMaterial( s_material[mat] );
    m_ros[ro]->getRenderTechnique()->setParametersProvider( s_material[mat] );
}

Engine::RenderTechnique* Gizmo::makeRenderTechnique() {
    auto rt       = new Engine::RenderTechnique;
    auto plaincfg = Engine::ShaderConfigurationFactory::getConfiguration( "Plain" );
    rt->setConfiguration( *plaincfg );
    return rt;
}

} // namespace Gui
} // namespace Ra
