#include <Engine/System/System.hpp>

#include <cstdio>

namespace Ra
{

System::System()
{
}

System::~System()
{
    m_components.clear();
}

void System::addComponent(Component *component, uint id)
{
    char buff[100];
    snprintf(buff, 100, "A component of id %ud has already been added to the system.", id);
    assert(m_components.find(id) != m_components.end() && buff);

    m_components[id] = component;
}

void System::removeComponent(uint id)
{
    char buff[100];
    snprintf(buff, 100, "The component of id %ud does not exist in the system.", id);
    assert(m_components.find(id) != m_components.end() && buff);

    m_components.erase(m_components.find(id));
}

} // namespace Ra
