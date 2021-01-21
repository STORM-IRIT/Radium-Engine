#include <Gui/Utils/PickingManager.hpp>

#include <Core/Utils/Index.hpp>

namespace Ra {
namespace Gui {

PickingManager::PickingManager() {
    m_pickingResult.m_mode  = Engine::Renderer::Renderer::RO;
    m_pickingResult.m_roIdx = -1;
}

PickingManager::~PickingManager() {}

void PickingManager::setCurrent( const Engine::Renderer::Renderer::PickingResult& pr ) {
    m_pickingResult = pr;
}

const Engine::Renderer::Renderer::PickingResult& PickingManager::getCurrent() const {
    return m_pickingResult;
}

void PickingManager::clear() {
    m_pickingResult.m_mode  = Engine::Renderer::Renderer::RO;
    m_pickingResult.m_roIdx = Core::Utils::Index::Invalid();
    m_pickingResult.m_vertexIdx.clear();
    m_pickingResult.m_edgeIdx.clear();
    m_pickingResult.m_elementIdx.clear();
}

} // namespace Gui
} // namespace Ra
