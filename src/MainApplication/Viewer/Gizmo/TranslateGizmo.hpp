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
            TranslateGizmo(Engine::Component* c, const Core::Transform& worldTo, const Core::Transform& t, Mode mode);

            virtual void updateTransform( const Core::Transform& worldTo, const Core::Transform& t ) override;
            virtual void selectConstraint( int drawableIndex ) override;
            virtual void setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY);
            virtual Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY);
        private:
            Ra::Core::Vector3 m_startPoint;
            Ra::Core::Vector3 m_initialTrans;
            Core::Vector2 m_initialPix;
            int m_selectedAxis;
            bool m_start;
        };
    }
}
#endif // RADIUMENGINE_TRANSLATE_GIZMO_HPP_
