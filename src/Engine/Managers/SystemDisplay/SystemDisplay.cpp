#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

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
            CORE_ASSERT(getInstance()->getNumComponents() == 2,  
                        "This entity should have two components only");
            CORE_ASSERT(getInstance()->getTransformAsMatrix() == 
                        Core::Matrix4::Identity(), "Transform has been changed !");
            return static_cast<UiComponent*>(getInstance()->getComponent("UI"));
        }

#ifndef RA_DISABLE_DEBUG_DISPLAY
        DebugComponent* SystemEntity::dbgCmp()
        {
            CORE_ASSERT(getInstance()->getNumComponents() == 2,  
                        "This entity should have two components only");
            CORE_ASSERT(getInstance()->getTransformAsMatrix() == 
                        Core::Matrix4::Identity(), "Transform has been changed !");
            return static_cast<DebugComponent*>(getInstance()->getComponent("Debug"));
        }

        RenderObject* DebugComponent::getRenderObject( Core::Index idx )
        {
            return getRoMgr()->getRenderObject( idx ).get();
        }
#endif

        RA_SINGLETON_IMPLEMENTATION( SystemEntity );
    }
}
