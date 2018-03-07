#ifndef RADIUMENGINE_ROTATE_GIZMO_HPP_
#define RADIUMENGINE_ROTATE_GIZMO_HPP_

#include <GuiBase/Viewer/Gizmo/Gizmo.hpp>

namespace Ra {
namespace Gui {
class RotateGizmo : public Gizmo {
  public:
    // Draw a frame with : 3 RGB arrows
    RotateGizmo( Engine::Component* c, const Core::Transform& worldTo, const Core::Transform& t,
                 Mode mode );

    virtual void updateTransform( Gizmo::Mode mode, const Core::Transform& worldTo,
                                  const Core::Transform& t ) override;
    virtual void selectConstraint( int drawableIndex ) override;
    virtual void setInitialState( const Engine::Camera& cam,
                                  const Core::Vector2& initialXY ) override;
    virtual Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY,
                                       bool stepped = false ) override;

  private:
    Core::Vector2 m_initialPix;
    int m_selectedAxis;
    bool m_start;
    bool m_stepped;
    float m_totalAngle;
    Core::Matrix3 m_initialRot;
};
} // namespace Gui
} // namespace Ra
#endif // RADIUMENGINE_ROTATE_GIZMO_HPP_
