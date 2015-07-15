#include <Engine/Entity/Entity.hpp>

#include <Core/CoreMacros.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{

Entity::Entity()
    : IndexedObject()
    , m_transform(Transform::Identity())
{
}

Entity::~Entity()
{
}

void Entity::addComponent(Component* component)
{
    Index idx = component->idx;

    char err[100];
    snprintf(err, 100, "Component %d has already been added to the entity.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) == m_components.end(), err);

    m_components.insert(ComponentByIndex(idx, component));

    component->setEntity(this);
}

Component* Entity::getComponent(Index idx)
{
    CORE_ASSERT(idx != Index::INVALID_IDX(), "Trying to access an invalid component");

    Component* comp = nullptr;

    auto it = m_components.find(idx);
    if (it != m_components.end())
    {
        comp = it->second;
    }

    return comp;
}

void Entity::removeComponent(Index idx)
{
    char buff[100];
    snprintf(buff, 100, "The component of id %ud is not part of the entity.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) != m_components.end(), buff);

    m_components.erase(idx);
}

void Entity::removeComponent(Component* component)
{
    removeComponent(component->idx);
}

} // namespace Ra
