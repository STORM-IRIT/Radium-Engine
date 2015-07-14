#include <Engine/Entity/System.hpp>

#include <cstdio>

#include <Engine/Entity/Component.hpp>

namespace Ra
{

System::System()
{
}

System::~System()
{
    m_components.clear();
}

void System::addComponent(Component* component)
{
    Index idx = component->idx;
    char buff[100];
    snprintf(buff, 100, "A component of index %u has already been added to the system.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) == m_components.end(), buff);

    m_components[idx] = component;

    component->setSystem(this);
}

void System::removeComponent(Index idx)
{
    char buff[100];
    snprintf(buff, 100, "The component of id %ud does not exist in the system.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) != m_components.end(), buff);

    m_components.erase(m_components.find(idx));
}

void System::removeComponent(Component* component)
{
    removeComponent(component->idx);
}

} // namespace Ra
