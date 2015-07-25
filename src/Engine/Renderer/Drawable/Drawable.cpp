#include <Engine/Renderer/Drawable/Drawable.hpp>

#include <Engine/Renderer/Material/Material.hpp>

namespace Ra
{

Engine::Drawable::Drawable(const std::string& name)
	: m_name(name)
	, m_material(nullptr)
	, m_isDirty(true)
{
}

Engine::Drawable::~Drawable()
{
}

void Engine::Drawable::updateGL()
{
	// Do not update while we are cloning
	std::lock_guard<std::mutex> lock(m_updateMutex);
	updateGLInternal();
}

Engine::Drawable* Engine::Drawable::clone()
{
	// Do not clone while we are updating GL internals
	std::lock_guard<std::mutex> lock(m_updateMutex);
	return cloneInternal();
}

}