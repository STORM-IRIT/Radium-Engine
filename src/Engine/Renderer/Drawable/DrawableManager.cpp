#include <Engine/Renderer/Drawable/DrawableManager.hpp>

#include <Core/CoreMacros.hpp>

#include <Engine/Renderer/Drawable/Drawable.hpp>

namespace Ra
{

Engine::DrawableManager::DrawableManager()
{

}

Engine::DrawableManager::~DrawableManager()
{

}

std::vector<std::shared_ptr<Engine::Drawable>> Engine::DrawableManager::getDrawables() const
{
    // Take the mutex
    std::lock_guard<std::mutex> lock(m_doubleBufferMutex);

    // Copy each element in m_drawables
    std::vector<std::shared_ptr<Drawable>> drawables;

    for (const auto& drawable : m_drawables)
    {
        drawables.push_back(drawable);
    }

    return drawables;
}

std::shared_ptr<Engine::Drawable> Engine::DrawableManager::update(uint index)
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
