#include <Engine/SystemDisplay/SystemDisplay.hpp>

namespace Ra
{
    namespace Engine
    {

        SystemEntity::SystemEntity()
        : Entity("System Display Entity")
        {
            addComponent( new UiComponent );
            addComponent( new DebugComponent );
        }

        UiComponent* SystemEntity::uiCmp()
        {
            CORE_ASSERT(getInstance()->getNumComponents() == 2,  "This entity should have two components only");
            CORE_ASSERT(getInstance()->getTransformAsMatrix() == Core::Matrix4::Identity(), "Transform has been changed !");
            return static_cast<UiComponent*>(getInstance()->getComponent("UI"));
        }

#ifndef RA_DISABLE_DEBUG_DISPLAY
        DebugComponent* SystemEntity::dbgCmp()
        {
            CORE_ASSERT(getInstance()->getNumComponents() == 2,  "This entity should have two components only");
            CORE_ASSERT(getInstance()->getTransformAsMatrix() == Core::Matrix4::Identity(), "Transform has been changed !");
            return static_cast<DebugComponent*>(getInstance()->getComponent("Debug"));
        }

        RenderObject* DebugComponent::updateRenderObject( Core::Index idx )
        {
            return getRoMgr()->update( idx ).get();
        }

        void DebugComponent::doneUpdatingRenderObject( Core::Index idx )
        {
            getRoMgr()->doneUpdating( idx );
        }

#endif

        RA_SINGLETON_IMPLEMENTATION(SystemEntity)
    }
}
