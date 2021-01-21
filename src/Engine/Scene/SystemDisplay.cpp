#include <Engine/Scene/RenderObjectManager.hpp>
#include <Engine/Scene/SystemDisplay.hpp>
namespace Ra {
namespace Engine {
namespace Scene {

SystemEntity::SystemEntity() : Entity( "System Display Entity" ) {
    /*
    addComponent(new UiComponent(nullptr));
    addComponent(new DebugComponent(nullptr));
     */
    new UiComponent( this );
    new DebugComponent( this );
}

UiComponent* SystemEntity::uiCmp() {
    SystemEntity* instance = getInstance();
    CORE_ASSERT( instance->getTransformAsMatrix() == Core::Matrix4::Identity(),
                 "Transform has been changed !" );
    return static_cast<UiComponent*>( instance->getComponent( "UI" ) );
}

#ifndef RA_DISABLE_DEBUG_DISPLAY
DebugComponent* SystemEntity::dbgCmp() {
    SystemEntity* instance = getInstance();
    CORE_ASSERT( instance->getTransformAsMatrix() == Core::Matrix4::Identity(),
                 "Transform has been changed !" );
    return static_cast<DebugComponent*>( instance->getComponent( "Debug" ) );
}

Renderer::RenderObject* DebugComponent::getRenderObject( Core::Utils::Index idx ) {
    return getRoMgr()->getRenderObject( idx ).get();
}
#endif

RA_SINGLETON_IMPLEMENTATION( SystemEntity );

} // namespace Scene
} // namespace Engine
} // namespace Ra
