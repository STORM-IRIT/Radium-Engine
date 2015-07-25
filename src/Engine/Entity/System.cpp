#include <Engine/Entity/System.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{

Engine::System::System(RadiumEngine* engine)
	: m_engine(engine)
{
}

Engine::System::~System()
{
    m_components.clear();
}

void Engine::System::addComponent(Engine::Component* component)
{
    Core::Index idx = component->idx;

    std::string err;
    Core::StringUtils::stringPrintf(err, "A component of index %u has already been added to the system.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) == m_components.end(), err.c_str());

    m_components[idx] = component;

    component->setSystem(this);
}

void Engine::System::removeComponent(Core::Index idx)
{
    std::string err;
    Core::StringUtils::stringPrintf(err, "The component of id %ud does not exist in the system.", idx.getValue());
    CORE_ASSERT(m_components.find(idx) != m_components.end(), err.c_str());

    m_components.erase(m_components.find(idx));
}

void Engine::System::removeComponent(Engine::Component* component)
{
    removeComponent(component->idx);
}

} // namespace Ra
