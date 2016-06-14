#include <Engine/Managers/SignalManager/SignalManager.hpp>

namespace Ra {
namespace Engine {


void SignalManager::fireEntityCreated( const ItemEntry& entity ) const
{
    CORE_ASSERT( entity.isEntityNode(), "Invalid entry");
    callFunctions(m_entityCreatedCallbacks, entity);
}

void SignalManager::fireEntityDestroyed( const ItemEntry& entity ) const
{
    CORE_ASSERT( entity.isEntityNode(), "Invalid entry");
    callFunctions(m_entityDestroyedCallbacks, entity);
}

void SignalManager::fireComponentAdded( const ItemEntry& component) const
{
    CORE_ASSERT( component.isComponentNode(), "Invalid entry");
    callFunctions(m_componentAddedCallbacks, component);
}

void SignalManager::fireComponentRemoved( const ItemEntry& component ) const
{
    CORE_ASSERT( component.isComponentNode(), "Invalid entry");
    callFunctions(m_componentRemovedCallbacks, component);
}

void SignalManager::fireRenderObjectAdded( const ItemEntry& ro ) const
{
    CORE_ASSERT( ro.isRoNode(), "Invalid entry");
    callFunctions(m_roAddedCallbacks, ro);
}
void SignalManager::fireRenderObjectRemoved( const ItemEntry& ro ) const
{
    CORE_ASSERT( ro.isRoNode(), "Invalid entry");
    callFunctions(m_roRemovedCallbacks, ro);
}
void SignalManager::callFunctions(const std::vector<Callback>& funcs, const ItemEntry& arg) const
{
    if (m_isOn) { for (const auto& f : funcs) { f(arg); } }
}
}}
