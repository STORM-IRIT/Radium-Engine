#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <vector>

#include <Core/Math/Types.hpp>
#include <Core/Utils/Index.hpp>
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
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Gizmo( Engine::Component* c, const Core::Transform& worldTo, const Core::Transform& t,
           Mode mode );

    virtual ~Gizmo();

    // shows or hides the gizmos drawables.
    void show( bool on );

    /// Called every time the underlying transform may have changed.
    virtual void updateTransform( Mode mode, const Core::Transform& worldTo,
                                  const Core::Transform& t ) = 0;

    /// Called when one of the drawables of the gizmo has been selected.
    virtual void selectConstraint( int drawableIndex ) = 0;

    /// Called when the gizmo is first clicked, with the camera parameters and the initial pixel
    /// coordinates.
    virtual void setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY ) = 0;

    /// Called when the mose movement is recorder with the camera parameters and the current pixel
    /// coordinates.
    virtual Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY,
                                       bool stepped = false ) = 0;

  protected:
    static bool findPointOnAxis( const Engine::Camera& cam, const Core::Vector3& origin,
                                 const Core::Vector3& axis, const Core::Vector2& pix,
                                 Core::Vector3& pointOut, std::vector<Scalar>& hits );

    static bool findPointOnPlane( const Engine::Camera& cam, const Core::Vector3& origin,
                                  const Core::Vector3& axis, const Core::Vector2& pix,
                                  Core::Vector3& pointOut, std::vector<Scalar>& hits );

  protected:
    Core::Transform m_worldTo;   ///< World to local space where the transform lives.
    Core::Transform m_transform; ///< Transform to be edited.
    Engine::Component* m_comp;   ///< Engine Ui component.
    Mode m_mode;                 ///< local or global.
    std::vector<Core::Utils::Index> m_renderObjects; ///< ros for the gizmo.
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_GIZMO_HPP_
