#ifndef RADIUMENGINE_TRANSLATE_GIZMO_HPP_
#define RADIUMENGINE_TRANSLATE_GIZMO_HPP_

#include <MainApplication/Viewer/Gizmo/Gizmo.hpp>
namespace Ra
{
    namespace Gui
    {
        class TranslateGizmo : public Gizmo
        {
        public:
            // Draw a frame with : 3 RGB arrows
            TranslateGizmo(Engine::Component* c, const Core::Transform& t, Mode mode);

            virtual void updateTransform(const Core::Transform& t) override;
            virtual void selectConstraint( int drawableIndex ) override;
            virtual void setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY) override;
            virtual Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY) override;
        private:
            Core::Vector2 m_initialPix;
            int m_selectedAxis;
        };
    }
}
#endif // RADIUMENGINE_TRANSLATE_GIZMO_HPP_
