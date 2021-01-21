#pragma once

#include <Gui/RaGui.hpp>

#include <Engine/Renderer/Renderer.hpp>

namespace Ra {
namespace Gui {
class RA_GUI_API PickingManager
{
  public:
    PickingManager();
    ~PickingManager();

    void setCurrent( const Engine::Renderer::Renderer::PickingResult& pr );
    const Engine::Renderer::Renderer::PickingResult& getCurrent() const;

    void clear();

  private:
    /// The selected feature data.
    Engine::Renderer::Renderer::PickingResult m_pickingResult;
};

} // namespace Gui
} // namespace Ra
