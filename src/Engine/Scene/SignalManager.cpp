#include <Core/CoreMacros.hpp>
#include <Engine/Scene/ItemEntry.hpp>
#include <Engine/Scene/SignalManager.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

SignalManager::ItemObservable& SignalManager::getEntityCreatedNotifier() {
    return m_entityCreatedCallbacks;
}

SignalManager::ItemObservable& SignalManager::getEntityDestroyedNotifier() {
    return m_entityDestroyedCallbacks;
}

SignalManager::ItemObservable& SignalManager::getComponentCreatedNotifier() {
    return m_componentAddedCallbacks;
}

SignalManager::ItemObservable& SignalManager::getComponentDestroyedNotifier() {
    return m_componentRemovedCallbacks;
}

SignalManager::ItemObservable& SignalManager::getRenderObjectCreatedNotifier() {
    return m_roAddedCallbacks;
}

SignalManager::ItemObservable& SignalManager::getRenderObjectDestroyedNotifier() {
    return m_roRemovedCallbacks;
}

SignalManager::FrameObservable& SignalManager::getEndFrameNotifier() {
    return m_frameEndCallbacks;
}

void SignalManager::fireEntityCreated( const ItemEntry& entity ) const {
    CORE_ASSERT( entity.isEntityNode(), "Invalid entry" );
    notify<const ItemEntry&>( m_entityCreatedCallbacks, entity );
}

void SignalManager::fireEntityDestroyed( const ItemEntry& entity ) const {
    CORE_ASSERT( entity.isEntityNode(), "Invalid entry" );
    notify<const ItemEntry&>( m_entityDestroyedCallbacks, entity );
}

void SignalManager::fireComponentAdded( const ItemEntry& component ) const {
    CORE_ASSERT( component.isComponentNode(), "Invalid entry" );
    notify<const ItemEntry&>( m_componentAddedCallbacks, component );
}

void SignalManager::fireComponentRemoved( const ItemEntry& component ) const {
    CORE_ASSERT( component.isComponentNode(), "Invalid entry" );
    notify<const ItemEntry&>( m_componentRemovedCallbacks, component );
}

void SignalManager::fireRenderObjectAdded( const ItemEntry& ro ) const {
    CORE_ASSERT( ro.isRoNode(), "Invalid entry" );
    notify<const ItemEntry&>( m_roAddedCallbacks, ro );
}
void SignalManager::fireRenderObjectRemoved( const ItemEntry& ro ) const {
    CORE_ASSERT( ro.isRoNode(), "Invalid entry" );
    notify<const ItemEntry&>( m_roRemovedCallbacks, ro );
}

void SignalManager::fireFrameEnded() const {
    notify<>( m_frameEndCallbacks );
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
