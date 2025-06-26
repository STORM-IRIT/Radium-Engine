#include <Engine/Scene/DefaultCameraManager.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/CameraComponent.hpp>

#include <algorithm>

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
    //    m_data.emplace( cam->getType(), cam );
    m_data.push_back( cam );
}

void DefaultCameraStorage::remove( CameraComponent* cam ) {
    //    auto range = m_data.equal_range( cam->getType() );
    //    for ( auto i = range.first; i != range.second; ++i )
    //    {
    //        if ( i->second == cam )
    //        {
    //            m_data.erase( i );
    //            break;
    //        }
    //    }
    auto itr = std::find( m_data.begin(), m_data.end(), cam );
    if ( itr != m_data.end() ) m_data.erase( itr );
}

size_t DefaultCameraStorage::size() const {
    return m_data.size();
}

void DefaultCameraStorage::clear() {
    m_data.clear();
}

CameraComponent* DefaultCameraStorage::operator[]( unsigned int n ) {

    //    auto iterator = m_data.begin();
    //    std::advance( iterator, n );
    //    return iterator->second;
    return m_data[n];
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
