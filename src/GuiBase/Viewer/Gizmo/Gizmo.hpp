#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <vector>

#include <Core/Types.hpp>
#include <Core/Utils/Index.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace Ra {
namespace Engine {
class Camera;
class Component;
class RenderObject;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Gui {
/**
 * Base class for gizmos, i.e.\ graphic tools to manipulate a transform.
 */
class Gizmo {
  public:
    /**
     * Used to indicate whether the transform is expressed in object or world space.
     */
    enum Mode {
        LOCAL,
        GLOBAL,
    };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Gizmo( Engine::Component* c, const Core::Transform& worldTo, const Core::Transform& t,
           Mode mode );

    virtual ~Gizmo();

    /**
     * Toggle display of the gizmo.
     */
    void show( bool on );

    /**
     * Update the transform of the gizmo drawables.
     * \note Must be called every time the underlying transform may have changed.
     */
    virtual void updateTransform( Mode mode, const Core::Transform& worldTo,
                                  const Core::Transform& t ) = 0;

    /**
     * Notify the given gizmo drawable has been selected.
     * \note If \p drawableIndex < 0, then unselect all.
     */
    virtual void selectConstraint( int drawableIndex ) = 0;

    /**
     * Notify transform edition is about to start.
     * \param cam the camera data when edition starts.
     * \param initialXY the pixel coordinates when edition starts.
     */
    virtual void setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY ) = 0;

    /**
     * Notify the transform is being edited.
     * \param cam the current camera data.
     * \param nextXY the current mouse position.
     * \param stepped whether edition must be performed stepped.
     */
    virtual Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY,
                                       bool stepped = false ) = 0;

  protected:
    /**
     * Look for the point, on the axis defined by \p origin and \p axis, on which
     * the given pixel projects.
     * \return true if the point has been found, false otherwise
     *         (e.g.\ axis orthogonal to the camera plane).
     */
    static bool findPointOnAxis( const Engine::Camera& cam, const Core::Vector3& origin,
                                 const Core::Vector3& axis, const Core::Vector2& pix,
                                 Core::Vector3& pointOut, std::vector<Scalar>& hits );

    /**
     * Look for the point, on the plane defined by \p origin and \p axis, on which
     * the given pixel projects.
     * \return true if the point has been found, false otherwise
     *         (e.g.\ plane orthogonal to the camera plane).
     */
    static bool findPointOnPlane( const Engine::Camera& cam, const Core::Vector3& origin,
                                  const Core::Vector3& axis, const Core::Vector2& pix,
                                  Core::Vector3& pointOut, std::vector<Scalar>& hits );

  protected:
    Core::Transform m_worldTo;   ///< World space to local space transform.
    Core::Transform m_transform; ///< Transform to be edited.
    Engine::Component* m_comp;   ///< Engine's UiComponent.
    Mode m_mode;                 ///< Whether to display drawables as local or global.
    std::vector<Core::Utils::Index> m_renderObjects; ///< Drawables for the gizmo.
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_GIZMO_HPP_
