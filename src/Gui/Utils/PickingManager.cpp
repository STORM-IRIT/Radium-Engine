#include <Gui/Utils/PickingManager.hpp>

#include <Core/Utils/Index.hpp>

namespace Ra {
namespace Gui {

PickingManager::PickingManager() {
    m_pickingResult.m_mode  = Engine::Rendering::Renderer::RO;
    m_pickingResult.m_roIdx = -1;
}

PickingManager::~PickingManager() {}

void PickingManager::setCurrent( const Engine::Rendering::Renderer::PickingResult& pr ) {
    m_pickingResult = pr;
}

const Engine::Rendering::Renderer::PickingResult& PickingManager::getCurrent() const {
    return m_pickingResult;
}

void PickingManager::clear() {
    m_pickingResult.clear();
}

} // namespace Gui
} // namespace Ra
