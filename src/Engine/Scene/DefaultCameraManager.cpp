#include <Engine/Scene/DefaultCameraManager.hpp>

#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

DefaultCameraManager::DefaultCameraManager() {
    m_data = std::make_unique<DefaultCameraStorage>();
}

const Data::Camera* DefaultCameraManager::getCamera( size_t cam ) const {
    return ( *m_data )[cam];
}

void DefaultCameraManager::addCamera( Data::Camera* cam ) {
    registerComponent( cam->getEntity(), cam );
}

DefaultCameraStorage::DefaultCameraStorage() = default;

void DefaultCameraStorage::add( Data::Camera* cam ) {
    m_Cameras.emplace( cam->getType(), cam );
}

void DefaultCameraStorage::remove( Data::Camera* cam ) {
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

Data::Camera* DefaultCameraStorage::operator[]( unsigned int n ) {
    auto iterator = m_Cameras.begin();
    std::advance( iterator, n );
    return iterator->second;
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
