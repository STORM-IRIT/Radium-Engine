#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
namespace Ra::Engine {

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
    // static cast is safe here are we are sure that the returned component is a UiComponent.
    return static_cast<UiComponent*>( instance->getComponent( "UI" ) );
}

#ifndef RA_DISABLE_DEBUG_DISPLAY
DebugComponent* SystemEntity::dbgCmp() {
    SystemEntity* instance = getInstance();
    CORE_ASSERT( instance->getTransformAsMatrix() == Core::Matrix4::Identity(),
                 "Transform has been changed !" );
    // static cast is safe here are we are sure that the returned component is a DebugComponent.
    return static_cast<DebugComponent*>( instance->getComponent( "Debug" ) );
}

RenderObject* DebugComponent::getRenderObject(const Core::Utils::Index &idx ) {
    return getRoMgr()->getRenderObject( idx ).get();
}
#endif

RA_SINGLETON_IMPLEMENTATION( SystemEntity );
} // namespace Ra::Engine
