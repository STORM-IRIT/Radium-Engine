#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Managers/SignalManager/SignalManager.hpp>

#include <numeric> // for reduce
namespace Ra {
namespace Engine {
RenderObjectManager::RenderObjectManager() = default;

RenderObjectManager::~RenderObjectManager() = default;

bool RenderObjectManager::exists( const Core::Utils::Index& index ) const {
    return ( index.isValid() && m_renderObjects.contains( index ) );
}

Core::Utils::Index RenderObjectManager::addRenderObject( RenderObject* renderObject ) {
    // Avoid data race in the std::maps
    std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

    std::shared_ptr<RenderObject> newRenderObject( renderObject );
    Core::Utils::Index index = m_renderObjects.insert( newRenderObject );

    newRenderObject->setIndex( index );

    auto type = renderObject->getType();

    m_renderObjectByType[(int)type].insert( index );

    Engine::RadiumEngine::getInstance()->getSignalManager()->fireRenderObjectAdded( ItemEntry(
        renderObject->getComponent()->getEntity(), renderObject->getComponent(), index ) );
    return index;
}

void RenderObjectManager::removeRenderObject( const Core::Utils::Index& index ) {
    CORE_ASSERT( exists( index ), "Trying to access a render object which doesn't exist" );

    // FIXME : Should we check if the render object is in the double buffer map ?
    std::shared_ptr<RenderObject> renderObject = m_renderObjects.at( index );

    Engine::RadiumEngine::getInstance()->getSignalManager()->fireRenderObjectRemoved( ItemEntry(
        renderObject->getComponent()->getEntity(), renderObject->getComponent(), index ) );

    // Lock after signal has been fired (as this signal can cause another RO to be deleted)
    std::lock_guard<std::mutex> lock( m_doubleBufferMutex );
    m_renderObjects.remove( index );

    auto type = renderObject->getType();
    m_renderObjectByType[(int)type].erase( index );
    renderObject.reset();
}

size_t RenderObjectManager::getRenderObjectsCount() {
    return m_renderObjects.size();
}

std::shared_ptr<RenderObject>
RenderObjectManager::getRenderObject( const Core::Utils::Index& index ) {
    CORE_ASSERT( exists( index ), "Trying to access a render object which doesn't exist" );
    return m_renderObjects.at( index );
}

void RenderObjectManager::getRenderObjects(
    std::vector<std::shared_ptr<RenderObject>>& renderObjectsOut ) const {
    // Take the mutex
    std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

    // Copy each element in m_renderObjects
    renderObjectsOut.resize( m_renderObjects.size() );
    std::copy( m_renderObjects.begin(), m_renderObjects.end(), renderObjectsOut.begin() );
}

void RenderObjectManager::getRenderObjectsByType(
    std::vector<std::shared_ptr<RenderObject>>& objectsOut, const RenderObjectType& type ) const {
    // Take the mutex
    std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

    //// Copy each element in m_renderObjects
    std::transform( m_renderObjectByType[(int)type].begin(), m_renderObjectByType[(int)type].end(),
                    std::back_inserter( objectsOut ), [this]( const Core::Utils::Index& i ) {
                        return this->m_renderObjects.at( i );
                    } );
}

void RenderObjectManager::renderObjectExpired( const Core::Utils::Index& idx ) {
    std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

    auto ro = m_renderObjects.at( idx );
    m_renderObjects.remove( idx );

    auto type = ro->getType();

    m_renderObjectByType[size_t( type )].erase( idx );

    ro->hasExpired();

    ro.reset();
}

size_t RenderObjectManager::getNumFaces() const {
    // todo : use reduce instead of accumulate to improve performances (since C++17)
    size_t result = std::accumulate(
        m_renderObjects.begin(), m_renderObjects.end(), size_t( 0 ),
        []( size_t a, const std::shared_ptr<RenderObject>& ro ) -> size_t {
            if ( ro->isVisible() && ro->getType() == Ra::Engine::RenderObjectType::Geometry )
            {
                return a + ro->getMesh()->getNumFaces();
            } else
            { return a; }
        } );
    return result;
}

size_t RenderObjectManager::getNumVertices() const {
    // todo : use reduce instead of accumulate to improve performances (since C++17)
    size_t result = std::accumulate(
        m_renderObjects.begin(), m_renderObjects.end(), size_t( 0 ),
        []( size_t a, const std::shared_ptr<RenderObject>& ro ) -> size_t {
            if ( ro->isVisible() && ro->getType() == Ra::Engine::RenderObjectType::Geometry )
            {
                return a + ro->getMesh()->getNumVertices();
            } else
            { return a; }
        } );
    return result;
}

Core::Aabb RenderObjectManager::getSceneAabb() const {
    using Ra::Core::Transform;
    Core::Aabb aabb;

    const auto& systemEntity = Engine::SystemEntity::getInstance();
    const auto& comps = systemEntity->getComponents();
    uint nUiRO = 0;
    std::for_each( comps.begin(), comps.end(),
                   [&nUiRO]( const auto& c ) { nUiRO += c->m_renderObjects.size(); } );
    if ( m_renderObjects.size() == nUiRO )
        return aabb;

    for ( const auto& ro : m_renderObjects )
    {
        auto entity = ro->getComponent()->getEntity();
        if ( ro->isVisible() && ( entity != systemEntity ) )
        {
            Transform tr = entity->getTransform() * ro->getLocalTransform();
            aabb.extend(
                Core::Geometry::Obb( ro->getMesh()->getGeometry().computeAabb(), tr ).toAabb() );
        }
    }
    return aabb;
}
} // namespace Engine
} // namespace Ra
