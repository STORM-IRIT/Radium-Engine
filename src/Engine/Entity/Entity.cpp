#include <Engine/Entity/Entity.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{

Engine::Entity::Entity()
    : Core::IndexedObject()
    , m_transform(Core::Transform::Identity())
{
}

Engine::Entity::~Entity()
{
}

void Engine::Entity::addComponent(Engine::Component* component)
{
    Core::Index idx = component->idx;

    char err[100];
    snprintf(err, 100, "Component %d has already been added to the entity.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) == m_components.end(), err);

    m_components.insert(ComponentByIndex(idx, component));

    component->setEntity(this);
}

Engine::Component* Engine::Entity::getComponent(Core::Index idx)
{
    CORE_ASSERT(idx != Core::Index::INVALID_IDX(), "Trying to access an invalid component");

    Engine::Component* comp = nullptr;

    auto it = m_components.find(idx);
    if (it != m_components.end())
    {
        comp = it->second;
    }

    return comp;
}

void Engine::Entity::removeComponent(Core::Index idx)
{
    std::string err;
    Core::StringUtils::stringPrintf(err, "The component of id %ud is not part of the entity.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) != m_components.end(), err.c_str());

    m_components.erase(idx);
}

void Engine::Entity::removeComponent(Engine::Component* component)
{
    removeComponent(component->idx);
}

} // namespace Ra
