namespace Ra
{

inline const std::string& Engine::Drawable::getName() const
{
	return m_name;
}

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

inline void Engine::Drawable::setMaterial(Material* material)
{
	m_material = std::shared_ptr<Material>(material); 
}

inline void Engine::Drawable::setMaterial(std::shared_ptr<Material> material)
{
	m_material = material;
}

} // namespace Ra
