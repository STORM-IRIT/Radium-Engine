#include <Engine/Component/Component.hpp>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Utils/Log.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/System/System.hpp>

namespace Ra {
namespace Engine {

using namespace Core::Utils; // log

Component::Component( const std::string& name, Entity* entity ) : m_name {name}, m_entity {entity} {
    m_entity->addComponent( this );
}

Component::~Component() {
    for ( const auto& ro : m_renderObjects )
    {
        getRoMgr()->removeRenderObject( ro );
    }
    if ( m_system ) { m_system->unregisterComponent( getEntity(), this ); }
    RadiumEngine::getInstance()->getSignalManager()->fireComponentRemoved(
        ItemEntry( getEntity(), this ) );
}

RenderObjectManager* Component::getRoMgr() {
    return RadiumEngine::getInstance()->getRenderObjectManager();
}

Core::Utils::Index Component::addRenderObject( RenderObject* renderObject ) {
    m_renderObjects.push_back( getRoMgr()->addRenderObject( renderObject ) );
    return m_renderObjects.back();
}

void Component::removeRenderObject( const Core::Utils::Index& roIdx ) {
    auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(), roIdx );
    CORE_WARN_IF( found == m_renderObjects.cend(), " Render object not found in component" );
    if ( ( found != m_renderObjects.cend() ) && getRoMgr() )
    {
        getRoMgr()->removeRenderObject( *found );
        m_renderObjects.erase( found );
    }
}

void Component::notifyRenderObjectExpired( const Core::Utils::Index& idx ) {
    auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(), idx );
    CORE_WARN_IF( found == m_renderObjects.cend(), " Render object not found in component" );
    if ( found != m_renderObjects.cend() ) { m_renderObjects.erase( found ); }
}

Core::Aabb Component::computeAabb() const {

    // component haven't any transformation, so ask render objects own aabb.
    Core::Aabb aabb;
    for ( const auto& roIndex : m_renderObjects )
    {
        auto ro = RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( roIndex );
        if ( ro->isVisible() ) { aabb.extend( ro->computeAabb() ); }
    }
    return aabb;
}

} // namespace Engine
} // namespace Ra
