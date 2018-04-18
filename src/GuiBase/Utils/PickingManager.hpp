#ifndef RADIUMENGINE_PICKINGMANAGER_HPP
#define RADIUMENGINE_PICKINGMANAGER_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <Engine/Renderer/Renderer.hpp>

#include <QObject>

namespace Ra {
namespace Gui {
class RA_GUIBASE_API PickingManager {
  public:
    PickingManager();
    ~PickingManager();

    void setCurrent( const Engine::Renderer::PickingResult& pr );
    const Engine::Renderer::PickingResult& getCurrent() const;

    void clear();

  private:
    /// The selected feature data.
    Engine::Renderer::PickingResult m_pickingResult;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_PICKINGMANAGER_HPP
