#include <Engine/Entity/Component.hpp>

namespace Ra
{

    inline void Engine::Component::setEntity( Engine::Entity* entity )
    {
        m_entity = entity;
    }

    inline Engine::Entity* Engine::Component::getEntity() const
    {
        return m_entity;
    }

    inline const std::string& Engine::Component::getName() const
    {
        return m_name;
    }

} // namespace Ra
