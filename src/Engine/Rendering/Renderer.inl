#pragma once
#include <Engine/Rendering/Renderer.hpp>

namespace Ra {
namespace Engine {
namespace Rendering {

inline const std::vector<std::tuple<int, int, int>>& Renderer::PickingResult::getIndices() const {
    return m_indices;
}

inline void Renderer::PickingResult::addIndex( const std::tuple<int, int, int>& idx ) {
    m_indices.push_back( idx );
}

inline void Renderer::PickingResult::reserve( size_t s ) {
    m_indices.reserve( s );
}

inline void Renderer::PickingResult::removeDuplicatedIndices() const {
    std::sort( m_indices.begin(), m_indices.end() );
    m_indices.erase( std::unique( m_indices.begin(), m_indices.end() ), m_indices.end() );
}

inline void Renderer::PickingResult::clear() {
    m_mode  = Engine::Rendering::Renderer::RO;
    m_roIdx = Core::Utils::Index::Invalid();
    m_indices.clear();
}

inline void Renderer::PickingResult::setRoIdx( Core::Utils::Index idx ) {
    m_roIdx = idx;
}

inline Core::Utils::Index Renderer::PickingResult::getRoIdx() const {
    return m_roIdx;
}

inline void Renderer::PickingResult::setMode( Renderer::PickingMode mode ) {
    m_mode = mode;
}

inline Renderer::PickingMode Renderer::PickingResult::getMode() const {
    return m_mode;
}

inline Scalar Renderer::PickingResult::getDepth() const {
    return m_depth;
}

inline void Renderer::PickingResult::setDepth( Scalar depth ) {
    m_depth = depth;
}

inline const Renderer::TimerData& Renderer::getTimerData() const {
    return m_timerData;
}

inline Data::Texture* Renderer::getDisplayTexture() const {
    return m_displayedTexture;
}

inline void Renderer::lockRendering() {
    m_renderMutex.lock();
}

inline void Renderer::unlockRendering() {
    m_renderMutex.unlock();
}

inline void Renderer::toggleWireframe() {
    m_wireframe = !m_wireframe;
}

inline void Renderer::enableWireframe( bool enabled ) {
    m_wireframe = enabled;
}

inline void Renderer::toggleDrawDebug() {
    m_drawDebug = !m_drawDebug;
}

inline void Renderer::enableDebugDraw( bool enabled ) {
    m_drawDebug = enabled;
}

inline void Renderer::enablePostProcess( bool enabled ) {
    m_postProcessEnabled = enabled;
}

inline void Renderer::addPickingRequest( const PickingQuery& query ) {
    m_pickingQueries.push_back( query );
}

inline const std::vector<Renderer::PickingResult>& Renderer::getPickingResults() const {
    return m_pickingResults;
}

inline const std::vector<Renderer::PickingQuery>& Renderer::getPickingQueries() const {
    return m_lastFramePickingQueries;
}

inline void Renderer::setMousePosition( const Core::Vector2& pos ) {
    m_mousePosition[0] = pos[0];
    m_mousePosition[1] = m_height - pos[1];
}

inline void Renderer::setBrushRadius( Scalar brushRadius ) {
    m_brushRadius = brushRadius;
}

inline Scalar Renderer::getBrushRadius() {
    return m_brushRadius;
}

inline void Renderer::setBackgroundColor( const Core::Utils::Color& color ) {
    m_backgroundColor = color;
}

inline const Core::Utils::Color& Renderer::getBackgroundColor() const {
    return m_backgroundColor;
}

} // namespace Rendering
} // namespace Engine
} // namespace Ra
