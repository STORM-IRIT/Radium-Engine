#ifndef RADIUMENGINE_SCALE_GIZMO_HPP_
#define RADIUMENGINE_SCALE_GIZMO_HPP_

#include <GuiBase/Viewer/Gizmo/Gizmo.hpp>
namespace Ra {
namespace Gui {
class ScaleGizmo : public Gizmo {
  public:
    // Draw a frame with : 3 RGB arrows
    ScaleGizmo( Engine::Component* c, const Core::Transform& worldTo, const Core::Transform& t,
                Mode mode );

    virtual void updateTransform( Gizmo::Mode mode, const Core::Transform& worldTo,
                                  const Core::Transform& t ) override;
    virtual void selectConstraint( int drawableIndex ) override;
    virtual void setInitialState( const Engine::Camera& cam,
                                  const Core::Vector2& initialXY ) override;
    virtual Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY,
                                       bool stepped = false ) override;

  private:
    Ra::Core::Vector3 m_prevScale;
    Ra::Core::Vector3 m_startPos;
    Ra::Core::Vector3 m_startPoint;
    Core::Vector2 m_initialPix;
    int m_selectedAxis;
    int m_selectedPlane;
    bool m_start;
    bool m_whole;
};
} // namespace Gui
} // namespace Ra
#endif // RADIUMENGINE_SCALE_GIZMO_HPP_
