#include <Engine/Entity/Component.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/System.hpp>

namespace Ra
{

void Engine::Component::setSystem(Engine::System* system)
{
    m_system = system;
}

void Engine::Component::setEntity(Engine::Entity* entity)
{
    m_entity = entity;
}

void Engine::Component::setSelected(bool selected)
{
    m_isSelected = selected;
}

} // namespace Ra
