#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>
#include <vector>


namespace Ra{ namespace Engine { class Component; }}
namespace Ra{ namespace Engine { class RenderObject; }}
namespace Ra{ namespace Engine { class Camera; }}

namespace Ra
{
        class Gizmo
        {
        public:
            enum Mode
            {
                LOCAL,
                GLOBAL,
            };

        public:
            RA_CORE_ALIGNED_NEW
            Gizmo(Engine::Component* c, const Core::Transform& t, Mode mode)
                    : m_transform(t), m_comp(c), m_mode(mode) { }

            virtual ~Gizmo();

            virtual void updateTransform( const Core::Transform& t ) = 0;
            virtual void selectConstraint( int drawableIndex ) = 0;
            virtual void setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY) = 0;
            virtual Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY) = 0;

        protected:
            Core::Transform m_transform;
            Engine::Component* m_comp;
            Mode m_mode;
            std::vector<Core::Index> m_renderObjects;
            
        };

        class TranslateGizmo : public Gizmo
        {
        public:
            // Draw a frame with : 3 RGB arrows
            TranslateGizmo(Engine::Component* c, const Core::Transform& t, Mode mode);

            void updateTransform(const Core::Transform& t) override;
            void selectConstraint( int drawableIndex ) override;
            void setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY);
            Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY);
        private:
            Core::Vector2 m_initialPix;
            int m_selectedAxis;
        };
}


#endif // RADIUMENGINE_GIZMO_HPP_
