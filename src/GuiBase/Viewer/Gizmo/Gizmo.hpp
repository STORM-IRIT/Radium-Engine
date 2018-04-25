#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <vector>

#include <Core/Container/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace Ra {
namespace Engine {
class Component;
}
} // namespace Ra
namespace Ra {
namespace Engine {
class RenderObject;
}
} // namespace Ra
namespace Ra {
namespace Engine {
class Camera;
}
} // namespace Ra

namespace Ra {
namespace Gui {
/// Base class for gizmos, i.e. graphic tools to manipulate a transform.
/// Do not feed after midnight.
class Gizmo {
  public:
    enum Mode {
        LOCAL,
        GLOBAL,
    };

  public:
    RA_CORE_ALIGNED_NEW
    Gizmo( Engine::Component* c, const Core::Math::Transform& worldTo, const Core::Math::Transform& t,
           Mode mode ) :
        m_worldTo( worldTo ),
        m_transform( t ),
        m_comp( c ),
        m_mode( mode ) {}

    virtual ~Gizmo() {
        for ( auto ro : m_renderObjects )
        {
            m_comp->removeRenderObject( ro );
        }
    }

    // shows or hides the gizmos drawables.
    void show( bool on ) {
        auto roMgr = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager();
        for ( auto ro : m_renderObjects )
        {
            roMgr->getRenderObject( ro )->setVisible( on );
        }
    }

    /// Called every time the underlying transform may have changed.
    virtual void updateTransform( Mode mode, const Core::Math::Transform& worldTo,
                                  const Core::Math::Transform& t ) = 0;

    /// Called when one of the drawables of the gizmo has been selected.
    virtual void selectConstraint( int drawableIndex ) = 0;

    /// Called when the gizmo is first clicked, with the camera parameters and the initial pixel
    /// coordinates.
    virtual void setInitialState( const Engine::Camera& cam, const Core::Math::Vector2& initialXY ) = 0;

    /// Called when the mose movement is recorder with the camera parameters and the current pixel
    /// coordinates.
    virtual Core::Math::Transform mouseMove( const Engine::Camera& cam, const Core::Math::Vector2& nextXY,
                                       bool stepped = false ) = 0;

  protected:
    Core::Math::Transform m_worldTo;   //! World to local space where the transform lives.
    Core::Math::Transform m_transform; //! Transform to be edited.
    Engine::Component* m_comp;   //! Engine Ui component
    Mode m_mode;                 //! local or global
    std::vector<Core::Container::Index> m_renderObjects;
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_GIZMO_HPP_
