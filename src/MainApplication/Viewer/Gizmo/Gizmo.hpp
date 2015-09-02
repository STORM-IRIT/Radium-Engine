#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <vector>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/RadiumEngine.hpp>

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

            virtual ~Gizmo()
            {
                for (auto ro : m_renderObjects)
                {
                    Engine::RadiumEngine::getInstance()->getRenderObjectManager()->removeRenderObject(ro->idx);
                }

            }

        protected:
            Core::Transform m_transform;
            Engine::Component* m_comp;
            std::vector<Engine::RenderObject*> m_renderObjects;
            
        };

        class TranslateGizmo : public Gizmo
        {
        public:
            // Draw a frame with : 3 RGB arrows
            TranslateGizmo(Engine::Component* c, const Core::Transform& t);
        };
}


#endif // RADIUMENGINE_GIZMO_HPP_
