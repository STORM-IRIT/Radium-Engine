#pragma once

#include <Gui/RaGui.hpp>

#include <Engine/Rendering/Renderer.hpp>

namespace Ra {
namespace Gui {
class RA_GUI_API PickingManager
{
  public:
    PickingManager();
    ~PickingManager();

    void setCurrent( const Engine::Rendering::Renderer::PickingResult& pr );
    const Engine::Rendering::Renderer::PickingResult& getCurrent() const;

    void clear();

  private:
    /// The selected feature data.
    Engine::Rendering::Renderer::PickingResult m_pickingResult;
};

} // namespace Gui
} // namespace Ra
