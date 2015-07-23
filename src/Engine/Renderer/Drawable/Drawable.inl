namespace Ra
{

inline void Engine::Drawable::setVisible(bool visible)
{
    std::lock_guard<std::mutex> lock(m_visibleMutex);
    m_visible = visible;
}

inline bool Engine::Drawable::isVisible() const
{
    std::lock_guard<std::mutex> lock(m_visibleMutex);
    return m_visible;
}

inline bool Engine::Drawable::isDirty() const
{
    return m_isDirty;
}

inline void Engine::Drawable::setComponent(Component* component)
{
    m_component = component;
}

} // namespace Ra
