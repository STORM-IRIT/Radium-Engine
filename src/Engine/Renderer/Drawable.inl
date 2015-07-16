namespace Ra
{

inline void Engine::Drawable::setVisible(bool visible)
{
    std::lock_guard<std::mutex> lock(m_visibleMutex);
    m_visible = visible;
}

inline bool Engine::Drawable::getVisible() const
{
    std::lock_guard<std::mutex> lock(m_visibleMutex);
    return m_visible;
}

} // namespace Ra
