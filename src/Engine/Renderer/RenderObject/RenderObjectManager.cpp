#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Managers/SignalManager/SignalManager.hpp>

namespace Ra {
namespace Engine {
RenderObjectManager::RenderObjectManager() {}

RenderObjectManager::~RenderObjectManager() {}

bool RenderObjectManager::exists( const Core::Index& index ) const {
    return ( index.isValid() && m_renderObjects.contains( index ) );
}

Core::Index RenderObjectManager::addRenderObject( RenderObject* renderObject ) {
    // Avoid data race in the std::maps
    std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

    std::shared_ptr<RenderObject> newRenderObject( renderObject );
    Core::Index index = m_renderObjects.insert( newRenderObject );

    newRenderObject->idx = index;

    auto type = renderObject->getType();

    m_renderObjectByType[(int)type].insert( index );

    Engine::RadiumEngine::getInstance()->getSignalManager()->fireRenderObjectAdded( ItemEntry(
        renderObject->getComponent()->getEntity(), renderObject->getComponent(), index ) );
    return index;
}

void RenderObjectManager::removeRenderObject( const Core::Index& index ) {
    CORE_ASSERT( exists( index ), "Trying to access a render object which doesn't exist" );

    // FIXME(Charly): Should we check if the render object is in the double buffer map ?
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

uint RenderObjectManager::getRenderObjectsCount() {
    return m_renderObjects.size();
}

std::shared_ptr<RenderObject> RenderObjectManager::getRenderObject( const Core::Index& index ) {
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
    for ( const auto& idx : m_renderObjectByType[(int)type] )
    {
        objectsOut.push_back( m_renderObjects.at( idx ) );
    }
}

void RenderObjectManager::renderObjectExpired( const Core::Index& idx ) {
    std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

    auto ro = m_renderObjects.at( idx );
    m_renderObjects.remove( idx );

    auto type = ro->getType();

    m_renderObjectByType[(int)type].erase( idx );

    ro->hasExpired();

    ro.reset();
}

uint RenderObjectManager::getNumFaces() const {
    uint result = 0;
    for ( const auto& ro : m_renderObjects )
    {
        if ( ro->isVisible() && ro->getType() == Ra::Engine::RenderObjectType::Fancy )
        {
            result += ro->getMesh()->getGeometry().m_triangles.size();
        }
    }
    return result;
}

uint RenderObjectManager::getNumVertices() const {
    uint result = 0;
    for ( const auto& ro : m_renderObjects )
    {
        if ( ro->isVisible() && ro->getType() == Ra::Engine::RenderObjectType::Fancy )
        {
            result += ro->getMesh()->getGeometry().vertices().size();
        }
    }
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

    for ( auto ro : m_renderObjects )
    {
        auto entity = ro->getComponent()->getEntity();
        if ( ro->isVisible() && ( entity != systemEntity ) )
        {
            Transform tr = entity->getTransform() * ro->getLocalTransform();
            for ( const auto& p : ro->getMesh()->getGeometry().vertices() )
            {
                aabb.extend( tr * p );
            }
        }
    }
    return aabb;
}
} // namespace Engine
} // namespace Ra
