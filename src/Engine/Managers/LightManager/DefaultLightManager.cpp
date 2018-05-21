#include "DefaultLightManager.hpp"

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra {
namespace Engine {

DefaultLightManager::DefaultLightManager() {
    m_data.reset( new DefaultLightStorage() );
}

const Light* DefaultLightManager::getLight( size_t li ) const {
    return ( *m_data )[li];
}

void DefaultLightManager::addLight( Light* li ) {
    m_data->add(li);
}
//
// Pre/Post render operations.
//

void DefaultLightManager::preprocess( const Ra::Engine::RenderData& rd ) {
    renderData = rd;
}

void DefaultLightManager::prerender( unsigned int li ) {
    Light* light = ( *m_data.get() )[li];
    params = RenderParameters();
    light->getRenderParameters( params );
}

void DefaultLightManager::render( RenderObject* ro, unsigned int li,
                                  RenderTechnique::PassName passname ) {
    ro->render( params, renderData, passname );
}

void DefaultLightManager::postrender( unsigned int li ) {}

void DefaultLightManager::postprocess() {
    // Eventually, this would be a good idea to disable GL_BLEND, and
    // all what was enabled in preprocess().
}

DefaultLightStorage::DefaultLightStorage() {}

void DefaultLightStorage::upload() const {}

void DefaultLightStorage::add(Light *li) {
    m_lights.emplace( li->getType(), li );
}

void DefaultLightStorage::remove(Light* li) {
    auto range = m_lights.equal_range(li->getType());
    for (auto i = range.first; i != range.second; ++i) {
        if (i->second == li) {
            m_lights.erase(i);
            break;
        }
    }
}

size_t DefaultLightStorage::size() const {
    return m_lights.size();
}

void DefaultLightStorage::clear() {
    m_lights.clear();
}

Light* DefaultLightStorage::operator[]( unsigned int n ) {
    auto iterator = m_lights.begin();
    std::advance( iterator, n );
    return iterator->second;
}
} // namespace Engine
} // namespace Ra
