namespace Ra
{

inline void Engine::Component::setSystem(Engine::System* system)
{
    m_system = system;
}

inline void Engine::Component::setEntity(Engine::Entity* entity)
{
    m_entity = entity;
}

inline const Engine::Entity* Engine::Component::getEntity() const
{
    return m_entity;
}

inline void Engine::Component::setDrawableManager(DrawableManager* manager)
{
    m_drawableManager = manager;
}

inline void Engine::Component::setSelected(bool selected)
{
    m_isSelected = selected;
}

inline const std::string& Engine::Component::getName() const
{
    return m_name;
}

} // namespace Ra
