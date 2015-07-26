#include <Engine/Renderer/Drawable/DrawableManager.hpp>

#include <Core/CoreMacros.hpp>

namespace Ra
{

Engine::DrawableManager::DrawableManager()
{

}

Engine::DrawableManager::~DrawableManager()
{
    fprintf(stderr, "DrawableManager::~DrawableManager\n");
}

Core::Index Engine::DrawableManager::addDrawable(Drawable* drawable)
{
	// Avoid data race in the std::maps
	std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

	std::shared_ptr<Drawable> newDrawable(drawable);
	return m_drawables.insert(newDrawable);
}

void Engine::DrawableManager::removeDrawable(const Core::Index & index)
{
	std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

	// FIXME(Charly): Should we check if the drawable is in 
	// the double buffer map ?
	std::shared_ptr<Drawable> drawable = m_drawables.at(index);
	m_drawables.remove(index);
	drawable.reset();
}

std::vector<std::shared_ptr<Engine::Drawable>> Engine::DrawableManager::getDrawables() const
{
    // Take the mutex
    std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    // Copy each element in m_drawables
    std::vector<std::shared_ptr<Drawable>> drawables;

	for (uint i = 0; i < m_drawables.size(); ++i)
	{
		drawables.push_back(m_drawables.at(i));
	}

    return drawables;
}

std::shared_ptr<Engine::Drawable> Engine::DrawableManager::update(uint index)
{
	Core::Index idx(index);

	return update(idx);
}

std::shared_ptr<Engine::Drawable> Engine::DrawableManager::update(const Core::Index& index)
{
    // A drawable should never be updated if it is already in use.
    // It might be :
    //     - update called by another component
    //     - doneUpdating not called on a previous update
    if (m_doubleBuffer.find(index) != m_doubleBuffer.end())
    {
        CORE_ERROR("The drawable required for update is already "
                   "being updated by someone else. "
                   "Maybe you forgot to call DrawableManager::doneUpdating() ?");
    }

    // Take the mutex
    std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    // Clone the drawable
    std::shared_ptr<Drawable> copy = std::shared_ptr<Drawable>(
                m_drawables[index]->clone());

    // Store it in the double buffer map
    m_doubleBuffer[index] = copy;
    return copy;
}

void Engine::DrawableManager::doneUpdating(uint index)
{
    // Take the mutex
    std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    std::shared_ptr<Drawable> oldDrawable = m_drawables[index];
    std::shared_ptr<Drawable> newDrawable = m_doubleBuffer[index];

    // We delete the old drawable. If it is still in used by the renderer,
    // the pointer will still be valid for it.
    oldDrawable.reset();

    // Buffer swapping
    m_drawables[index] = newDrawable;

    // Remove the double buffer entry
    m_doubleBuffer.erase(index);
}

} // namespace Ra
