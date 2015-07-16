#include <Engine/Entity/ComponentManager.hpp>

#include <Core/CoreMacros.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{

Engine::ComponentManager::~ComponentManager()
{
    for (uint i = 0; i < m_components.size(); ++i)
    {
        // FIXME (Charly): Check if every accessed idx is guaranted to exist.
        auto comp = m_components[i];
        CORE_ASSERT(comp.unique(), "Non-unique component about to be removed.");
        comp.reset();
    }

    m_components.clear();
}

void Engine::ComponentManager::addComponent(Engine::Component* component)
{
    CORE_ASSERT(component->idx == Core::Index::INVALID_IDX(),
                "The component has already been added to the Manager.");

    component->idx = m_components.insert(std::shared_ptr<Component>(component));
}

void Engine::ComponentManager::removeComponent(Core::Index idx)
{
    CORE_ASSERT(idx != Core::Index::INVALID_IDX() && m_components.contain(idx),
                "Trying to remove a component thas has not been added to the manager.");

    auto comp = m_components[idx];

    CORE_ASSERT(comp.unique(), "Non-unique component about to be removed.");

    comp.reset();
    m_components.remove(idx);
}

void Engine::ComponentManager::removeComponent(Engine::Component* component)
{
    removeComponent(component->idx);
}

Engine::Component* Engine::ComponentManager::getComponent(Core::Index idx) const
{
    CORE_ASSERT(idx != Core::Index::INVALID_IDX(), "Trying to access an invalid component.");

    Engine::Component* comp = nullptr;

    if (m_components.contain(idx))
    {
        comp = m_components.at(idx).get();
    }

    return comp;
}

} // namespace Ra
