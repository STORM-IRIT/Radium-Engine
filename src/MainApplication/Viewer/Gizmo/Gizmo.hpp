#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>
#include <vector>


namespace Ra{ namespace Core { struct Ray; }}
namespace Ra{ namespace Engine { class Component; }}
namespace Ra{ namespace Engine { class RenderObject; }}

namespace Ra
{
        class Gizmo
        {
        public:
            RA_CORE_ALIGNED_NEW
            Gizmo(Engine::Component* c, const Core::Transform& t)
                    : m_transform(t), m_comp(c) { }

            virtual ~Gizmo();

            virtual void updateTransform( const Core::Transform& t ) = 0;
            virtual void selectConstraint( const Core::Ray& ray ) = 0;

        protected:
            Core::Transform m_transform;
            Engine::Component* m_comp;
            std::vector<Core::Index> m_renderObjects;
            
        };

        class TranslateGizmo : public Gizmo
        {
        public:
            // Draw a frame with : 3 RGB arrows
            TranslateGizmo(Engine::Component* c, const Core::Transform& t);

            void updateTransform(const Core::Transform& t) override;
            void selectConstraint(const Core::Ray& ray) override;
        };
}


#endif // RADIUMENGINE_GIZMO_HPP_
