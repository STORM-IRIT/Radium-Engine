#ifndef RADIUMENGINE_DRAWABLEMANAGER_HPP
#define RADIUMENGINE_DRAWABLEMANAGER_HPP

#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>
#include <Core/CoreMacros.hpp>
#include <Engine/Renderer/Drawable/Drawable.hpp>

namespace Ra { namespace Engine {

class DrawableManager
{
public:
    DrawableManager();
    ~DrawableManager();

	Core::Index addDrawable(Drawable* drawable);
	void removeDrawable(const Core::Index& index);
	
	std::vector<std::shared_ptr<Drawable>> getDrawables() const;

	std::shared_ptr<Drawable> update(uint index);
    std::shared_ptr<Drawable> update(const Core::Index& index);
    void doneUpdating(uint index);

private:
    Core::IndexMap<std::shared_ptr<Drawable>> m_drawables;
    std::map<Core::Index, std::shared_ptr<Drawable>> m_doubleBuffer;

    mutable std::mutex m_doubleBufferMutex;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_DRAWABLEMANAGER_HPP
