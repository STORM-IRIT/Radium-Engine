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
    for (auto& component : m_components)
    {
        component.second.reset();
    }

    m_components.clear();
}

void Engine::System::addComponent(Engine::Component* component)
{
    std::shared_ptr<Component> comp(component);
    std::string name = component->getName();

    std::string err;
    Core::StringUtils::stringPrintf(err, "The component \"%s\" has already been added to the system.",
                                    name.c_str());
    CORE_ASSERT(m_components.find(name) == m_components.end(), err.c_str());

    m_components[name] = comp;

}

void Engine::System::removeComponent(const std::string& name)
{
    std::string err;
    Core::StringUtils::stringPrintf(err, "The component \"%s\" does not exist in the system.",
                                    name.c_str());
    CORE_ASSERT(m_components.find(name) != m_components.end(), err.c_str());

    std::shared_ptr<Component> component = m_components[name];
    component.reset();
    m_components.erase(name);
}

void Engine::System::removeComponent(Engine::Component* component)
{
    removeComponent(component->getName());
}

} // namespace Ra
