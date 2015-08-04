#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{

inline void Engine::RenderObject::setRenderObjectType(uint type)
{
    m_type = type;
}

inline uint Engine::RenderObject::getRenderObjectType() const
{
    return m_type;
}

inline const std::string& Engine::RenderObject::getName() const
{
	return m_name;
}

inline void Engine::RenderObject::setVisible(bool visible)
{
    m_visible = visible;
}

inline bool Engine::RenderObject::isVisible() const
{
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

inline void Engine::RenderObject::setRenderTechnique(RenderTechnique *technique)
{
    CORE_ASSERT(technique, "Passing a nullptr as render technique");
    m_renderTechnique = technique;
}

inline Engine::RenderTechnique* Engine::RenderObject::getRenderTechnique() const
{
    return m_renderTechnique;
}

inline void Engine::RenderObject::setMesh(Mesh* mesh)
{
    m_mesh = mesh;
}

inline Engine::Mesh* Engine::RenderObject::getMesh() const
{
    return m_mesh;
}

} // namespace Ra
