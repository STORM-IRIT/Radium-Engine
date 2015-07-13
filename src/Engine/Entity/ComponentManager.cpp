#include <Engine/Entity/ComponentManager.hpp>

#include <Core/CoreMacros.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{

ComponentManager::~ComponentManager()
{
    for (int i = 0; i < m_components.size(); ++i)
    {
        Index idx = m_components.index(i);

        if (m_components.contain(idx))
        {
            auto comp = m_components.access(idx);
            CORE_ASSERT(comp.unique(), "Non-unique component about to be removed.");
            comp.reset();
        }
    }

    m_components.clear();
}

void ComponentManager::addComponent(Component* component)
{
    CORE_ASSERT(component->getId() == Index::INVALID_IDX(),
                "The component has already been added to the Manager.");

    Index idx = m_components.insert(std::shared_ptr<Component>(component));
    component->setId(idx);
}

void ComponentManager::removeComponent(Index idx)
{
    CORE_ASSERT(idx != Index::INVALID_IDX(),
                "Trying to remove a component thas has not been added to the manager.");
    CORE_ASSERT(m_components.contain(idx),
                "Trying to remove a component that has not been added to the manager.");

    auto comp = m_components.access(idx);

    CORE_ASSERT(comp.unique(), "Non-unique component about to be removed.");

    comp.reset();
    m_components.remove(idx);
}

void ComponentManager::removeComponent(Component* component)
{
    removeComponent(component->getId());
}

Component* ComponentManager::getComponent(Index idx) const
{
    CORE_ASSERT(idx != Index::INVALID_IDX(), "Trying to access an invalid component.");

    Component* ret = nullptr;

    if (m_components.contain(idx))
    {
        ret = m_components.at(idx).get();
    }

    return ret;
}

} // namespace Ra
