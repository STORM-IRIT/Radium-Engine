#include <Engine/Entity/Component.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/System/System.hpp>

namespace Ra
{

Component::Component(uint id, System* system)
    : m_id(id)
    , m_system(system)
{
    m_system->addComponent(this, id);
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
