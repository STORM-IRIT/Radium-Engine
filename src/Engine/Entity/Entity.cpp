#include <Engine/Entity/Entity.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{

Engine::Entity::Entity(const std::string& name)
    : Core::IndexedObject()
    , m_transform(Core::Transform::Identity())
    , m_name(name)
{
}

Engine::Entity::~Entity()
{
}

void Engine::Entity::addComponent(Engine::Component* component)
{
    std::string name = component->getName();
    char err[100];
    snprintf(err, 100, "Component \"%s\" has already been added to the entity.", name.c_str());
    CORE_ASSERT(m_components.find(name) == m_components.end(), err);

    m_components.insert(ComponentByName(name, component));

    component->setEntity(this);
}

Engine::Component* Engine::Entity::getComponent(const std::string& name)
{
    Engine::Component* comp = nullptr;

    auto it = m_components.find(name);
    if (it != m_components.end())
    {
        comp = it->second;
    }

    return comp;
}

void Engine::Entity::removeComponent(const std::string& name)
{
    std::string err;
    Core::StringUtils::stringPrintf(err, "The component \"%s\" is not part of the entity \"%s\"",
                                    name.c_str(), m_name.c_str());
    CORE_ASSERT(m_components.find(name) != m_components.end(), err.c_str());

    m_components.erase(name);
}

void Engine::Entity::removeComponent(Engine::Component* component)
{
    removeComponent(component->getName());
}

std::vector<Engine::Component*> Engine::Entity::getComponents() const
{
    std::vector<Engine::Component*> components;

    for (const auto& comp : m_components)
    {
        components.push_back(comp.second);
    }

    return components;
}

void Engine::Entity::setSelected(bool selected)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isSelected = selected;

    for (auto& comp : m_components)
    {
        comp.second->setSelected(selected);
    }
}

} // namespace Ra
