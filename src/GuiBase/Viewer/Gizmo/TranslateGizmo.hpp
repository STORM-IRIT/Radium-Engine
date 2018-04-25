#ifndef RADIUMENGINE_TRANSLATE_GIZMO_HPP_
#define RADIUMENGINE_TRANSLATE_GIZMO_HPP_

#include <GuiBase/Viewer/Gizmo/Gizmo.hpp>
namespace Ra {
namespace Gui {
class TranslateGizmo : public Gizmo {
  public:
    // Draw a frame with : 3 RGB arrows
    TranslateGizmo( Engine::Component* c, const Core::Math::Transform& worldTo, const Core::Math::Transform& t,
                    Mode mode );

    virtual void updateTransform( Gizmo::Mode mode, const Core::Math::Transform& worldTo,
                                  const Core::Math::Transform& t ) override;
    virtual void selectConstraint( int drawableIndex ) override;
    virtual void setInitialState( const Engine::Camera& cam,
                                  const Core::Math::Vector2& initialXY ) override;
    virtual Core::Math::Transform mouseMove( const Engine::Camera& cam, const Core::Math::Vector2& nextXY,
                                       bool stepped = false ) override;

  private:
    Ra::Core::Math::Vector3 m_startPoint;
    Ra::Core::Math::Vector3 m_initialTrans;
    Core::Math::Vector2 m_initialPix;
    int m_selectedAxis;
    bool m_start;
};
} // namespace Gui
} // namespace Ra
#endif // RADIUMENGINE_TRANSLATE_GIZMO_HPP_
