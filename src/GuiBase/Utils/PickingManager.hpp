#ifndef RADIUMENGINE_PICKINGMANAGER_HPP
#define RADIUMENGINE_PICKINGMANAGER_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <Engine/Renderer/Renderer.hpp>

namespace Ra {
namespace Gui {

/**
 * The PickingManager stores picking results for Plugins.
 */
class RA_GUIBASE_API PickingManager {
  public:
    PickingManager();

    ~PickingManager();

    /**
     * Set the picking results from last frame.
     */
    void setCurrent( const Engine::Renderer::PickingResult& pr );

    /**
     * Return the picking results from last frame.
     */
    const Engine::Renderer::PickingResult& getCurrent() const;

    /**
     * Clear the picking results.
     */
    void clear();

  private:
    /// The picking results from last frame.
    Engine::Renderer::PickingResult m_pickingResult;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_PICKINGMANAGER_HPP
