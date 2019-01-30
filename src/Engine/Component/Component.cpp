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

Component::Component( const std::string& name, Entity* entity ) : m_name{name}, m_entity{entity} {
    m_entity->addComponent( this );
}

Component::~Component() {
    for ( const auto& ro : m_renderObjects )
    {
        getRoMgr()->removeRenderObject( ro );
    }
    if ( m_system )
    {
        m_system->unregisterComponent( getEntity(), this );
    }
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
    if ( found != m_renderObjects.cend() )
    {
        m_renderObjects.erase( found );
    }
}

void Component::rayCastQuery( const Ra::Core::Ray& ray ) const {
    for ( const auto& idx : m_renderObjects )
    {
        const auto ro = getRoMgr()->getRenderObject( idx );
        if ( ro->isVisible() )
        {
            const Ra::Core::Transform& t = ro->getLocalTransform();
            auto transformedRay = Core::Math::transformRay( t.inverse(), ray );
            auto result = ro->getMesh()->getGeometry().castRay( transformedRay );
            const int& tidx = result.m_hitTriangle;
            if ( tidx >= 0 )
            {

                const Ra::Core::Vector3 pLocal = transformedRay.pointAt( result.m_t );
                const Ra::Core::Vector3 pEntity = t * pLocal;
                const Ra::Core::Vector3 pWorld = getEntity()->getTransform() * pEntity;

                LOG( logINFO ) << " Ray cast vs " << ro->getName();
                LOG( logINFO ) << " Hit triangle " << tidx;
                LOG( logINFO ) << " Nearest vertex " << result.m_nearestVertex;
                LOG( logINFO ) << "Hit position (RO): " << pLocal.transpose();
                LOG( logINFO ) << "Hit position (Comp): " << pEntity.transpose();
                LOG( logINFO ) << "Hit position (World): " << pWorld.transpose();
            }
        }
    }
}
} // namespace Engine
} // namespace Ra
