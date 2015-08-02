#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{

inline const std::string& Engine::RenderObject::getName() const
{
	return m_name;
}

inline void Engine::RenderObject::setVisible(bool visible)
{
    std::lock_guard<std::mutex> lock(m_visibleMutex);
    m_visible = visible;
}

inline bool Engine::RenderObject::isVisible() const
{
    std::lock_guard<std::mutex> lock(m_visibleMutex);
    return m_visible;
}

inline bool Engine::RenderObject::isDirty() const
{
    return m_isDirty;
}

inline void Engine::RenderObject::setComponent(Component* component)
{
    m_component = component;
}

inline void Engine::RenderObject::setMaterial(Material* material)
{
	m_material = std::shared_ptr<Material>(material); 
}

inline Engine::Material* Engine::RenderObject::getMaterial() const
{
    return m_material.get();
}

inline void Engine::RenderObject::setMaterial(std::shared_ptr<Material> material)
{
	m_material = material;
}

inline const Core::Aabb& Engine::RenderObject::getBoundingBox() const
{
	return m_boundingBox;
}

} // namespace Ra
