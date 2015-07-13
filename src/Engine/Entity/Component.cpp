#include <Engine/Entity/Component.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/System.hpp>

namespace Ra
{

void Component::setSystem(System* system)
{
    m_system = system;
}

void Component::setEntity(Entity* entity)
{
    m_entity = entity;
}

void Component::setTransform(Transform* transform)
{
    m_transform = transform;
}

} // namespace Ra
