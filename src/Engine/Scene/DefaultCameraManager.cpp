#include <Engine/Scene/DefaultCameraManager.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/Camera.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

DefaultCameraManager::DefaultCameraManager() {
    m_data = std::make_unique<DefaultCameraStorage>();
}

const CameraComponent* DefaultCameraManager::getCamera( size_t cam ) const {
    return ( *m_data )[cam];
}

void DefaultCameraManager::addCamera( CameraComponent* cam ) {
    registerComponent( cam->getEntity(), cam );
}

DefaultCameraStorage::DefaultCameraStorage() = default;

void DefaultCameraStorage::add( CameraComponent* cam ) {
    m_Cameras.emplace( cam->getType(), cam );
}

void DefaultCameraStorage::remove( CameraComponent* cam ) {
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

CameraComponent* DefaultCameraStorage::operator[]( unsigned int n ) {
    auto iterator = m_Cameras.begin();
    std::advance( iterator, n );
    return iterator->second;
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
