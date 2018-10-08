#include "DefaultCameraManager.hpp"

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra {
namespace Engine {

DefaultCameraManager::DefaultCameraManager() {
    m_data.reset( new DefaultCameraStorage() );
}

const Camera* DefaultCameraManager::getCamera( size_t cam ) const {
    return ( *m_data )[cam];
}

void DefaultCameraManager::addCamera( Camera* cam ) {
    registerComponent( cam->getEntity(), cam );
}

void DefaultCameraManager::preprocess( const Ra::Engine::RenderData& rd ) {
    renderData = rd;
}

void DefaultCameraManager::render( RenderObject* ro, unsigned int cam,
                                   RenderTechnique::PassName passname ) {
    auto camera = getCamera( cam );
    renderData.projMatrix = camera->getProjMatrix();
    renderData.viewMatrix = camera->getViewMatrix();
    ro->render( params, renderData, passname );
}

DefaultCameraStorage::DefaultCameraStorage() {}

void DefaultCameraStorage::add( Camera* cam ) {
    m_Cameras.emplace( cam->getType(), cam );
}

void DefaultCameraStorage::remove( Camera* cam ) {
    auto range = m_Cameras.equal_range( cam->getType() );
    for ( auto i = range.first; i != range.second; ++i )
    {
        if ( i->second == cam )
        {
            m_Cameras.erase( i );
            break;
        }
    }
}

size_t DefaultCameraStorage::size() const {
    return m_Cameras.size();
}

void DefaultCameraStorage::clear() {
    m_Cameras.clear();
}

Camera* DefaultCameraStorage::operator[]( unsigned int n ) {
    auto iterator = m_Cameras.begin();
    std::advance( iterator, n );
    return iterator->second;
}

} // namespace Engine
} // namespace Ra
