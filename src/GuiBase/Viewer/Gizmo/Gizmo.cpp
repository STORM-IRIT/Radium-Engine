#include <GuiBase/Viewer/Gizmo/Gizmo.hpp>

#include <Core/Geometry/RayCast.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra {
namespace Gui {

Gizmo::Gizmo( Engine::Component* c, const Core::Transform& worldTo, const Core::Transform& t,
              Mode mode ) :
    m_worldTo( worldTo ),
    m_transform( t ),
    m_comp( c ),
    m_mode( mode ) {}

Gizmo::~Gizmo() {
    // first release meshes shared pointers before destroying ROs
    m_meshes.clear();
    for ( auto ro : m_renderObjects )
    {
        m_comp->removeRenderObject( ro );
    }
}

void Gizmo::show( bool on ) {
    auto roMgr = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager();
    for ( auto ro : m_renderObjects )
    {
        roMgr->getRenderObject( ro )->setVisible( on );
    }
}

bool Gizmo::findPointOnAxis( const Engine::Camera& cam, const Core::Vector3& origin,
                             const Core::Vector3& axis, const Core::Vector2& pix,
                             Core::Vector3& pointOut, std::vector<Scalar>& hits ) {
    // Taken from Rodolphe's View engine gizmos -- see slide_axis().
    // Find a plane containing axis and as parallel as possible to
    // the camera image plane
    const Core::Vector3 ortho = cam.getDirection().cross( axis );
    const Core::Vector3 normal =
        ( ortho.squaredNorm() > 0 ) ? axis.cross( ortho ) : axis.cross( cam.getUpVector() );

    const auto ray = cam.getRayFromScreen( pix );
    bool hasHit = Core::Geometry::RayCastPlane( ray, origin, normal, hits );
    if ( hasHit )
    {
        pointOut = origin + ( axis.dot( ray.pointAt( hits[0] ) - origin ) ) * axis;
    }
    return hasHit;
}

bool Gizmo::findPointOnPlane( const Engine::Camera& cam, const Core::Vector3& origin,
                              const Core::Vector3& axis, const Core::Vector2& pix,
                              Core::Vector3& pointOut, std::vector<Scalar>& hits ) {
    // Taken from Rodolphe's View engine gizmos -- see slide_plane().
    const auto ray = cam.getRayFromScreen( pix );
    bool hasHit = Core::Geometry::RayCastPlane( ray, origin, axis, hits );
    if ( hasHit )
    {
        pointOut = ray.pointAt( hits[0] );
    }
    return hasHit;
}

void Gizmo::addRenderObject( Engine::RenderObject* ro, const std::shared_ptr<Engine::Mesh>& mesh ) {
    m_meshes.push_back( mesh );
    m_renderObjects.push_back( m_comp->addRenderObject( ro ) );
}

} // namespace Gui
} // namespace Ra
