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

    inline void Engine::Component::setRenderObjectManager( RenderObjectManager* manager )
    {
        m_renderObjectManager = manager;
    }

    inline void Engine::Component::setSelected( bool selected )
    {
        m_isSelected = selected;
    }

    inline const std::string& Engine::Component::getName() const
    {
        return m_name;
    }

} // namespace Ra
