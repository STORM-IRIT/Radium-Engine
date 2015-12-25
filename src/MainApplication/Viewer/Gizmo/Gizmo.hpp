#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <vector>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>
#include <Engine/Entity/Component.hpp>


namespace Ra { namespace Engine { class Component; } }
namespace Ra { namespace Engine { class RenderObject; } }
namespace Ra { namespace Engine { class Camera; } }

namespace Ra
{
    namespace Gui
    {
        /// Base class for gizmos, i.e. graphic tools to manipulate a transform.
        /// Do not feed after midnight.
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

            virtual ~Gizmo()
            {
                for (auto ro : m_renderObjects)
                {
                    m_comp->removeRenderObject(ro);
                }
            }

            virtual void updateTransform(const Core::Transform& t) = 0;

            virtual void selectConstraint(int drawableIndex) = 0;

            virtual void setInitialState(const Engine::Camera& cam, const Core::Vector2& initialXY) = 0;

            virtual Core::Transform mouseMove(const Engine::Camera& cam, const Core::Vector2& nextXY) = 0;

        protected:
            Core::Transform m_transform;
            Engine::Component* m_comp;
            Mode m_mode;
            std::vector<Core::Index> m_renderObjects;

        };
    }
}


#endif // RADIUMENGINE_GIZMO_HPP_
