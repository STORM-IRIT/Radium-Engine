#include <Engine/DebugDisplay/DebugDisplay.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace Ra
{
    namespace Engine
    {
        DebugComponent::DebugComponent() : Component("Debug Display Component")
        {
        }


        void DebugComponent::addDebugDrawable(RenderObject* ro)
        {
            m_debugDrawableIndices.push_back(getRoMgr()->addRenderObject(ro));
        }

        DebugEntity::DebugEntity()
        : Entity("Debug Display Entity"), Core::Singleton<DebugEntity>()
        {
            addComponent(new DebugComponent);
        }

        DebugComponent* DebugEntity::getDebugComponent() const
        {
            CORE_ASSERT(getComponentsMap().size() == 1,  "This entity should have one component only");
            CORE_ASSERT(getTransformAsMatrix() == Core::Matrix4::Identity(), "Transform has been changed !");
            return static_cast<DebugComponent*>(getComponentsMap().begin()->second);
        }

    }
}
