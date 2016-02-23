#include <Engine/Managers/SignalManager/SignalManager.hpp>

namespace Ra {
namespace Engine {


void SignalManager::fireEntityCreated() const
{
    callFunctions(m_entityCreatedCallbacks);
}

void SignalManager::fireEntityDestroyed() const
{
    callFunctions(m_entityDestroyedCallbacks);
}

void SignalManager::fireComponentAdded() const
{
    callFunctions(m_componentAddedCallbacks);
}

void SignalManager::fireComponentRemoved() const
{
    callFunctions(m_componentRemovedCallbacks);
}

void SignalManager::callFunctions(const std::vector<Callback>& funcs)
{
    for (const auto& f : funcs) { f(); }
}

}}
