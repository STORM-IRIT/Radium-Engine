#include <Engine/Component/Component.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/System/System.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{
    namespace Engine
    {
        Component::Component( const std::string& name )
                : m_name( name )
                , m_entity( nullptr )
                , m_system( nullptr )
        {
        }

        Component::~Component()
        {
            for (const auto& ro : m_renderObjects )
            {
                getRoMgr()->removeRenderObject( ro );
            }
        }

        RenderObjectManager* Component::getRoMgr()
        {
            return RadiumEngine::getInstance()->getRenderObjectManager();
        }

        Core::Index Component::addRenderObject( RenderObject* renderObject )
        {
            m_renderObjects.push_back( getRoMgr()->addRenderObject( renderObject ) );
            return m_renderObjects.back();
        }

        void Component::removeRenderObject( Core::Index roIdx )
        {
            auto found = std::find(m_renderObjects.cbegin(), m_renderObjects.cend(),roIdx);
            CORE_WARN_IF(found == m_renderObjects.cend(), " Render object not found in component");
            if (found != m_renderObjects.cend() && getRoMgr() )
            {
                getRoMgr()->removeRenderObject(*found);
                m_renderObjects.erase(found);
            }
        }

        Core::Transform Component::getWorldTransform() const
        {
           return getEntity()->getTransform();
        }

        void Component::notifyRenderObjectExpired( const Core::Index& idx )
        {
            auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(), idx );
            CORE_WARN_IF( found == m_renderObjects.cend(), " Render object not found in component" );
            if ( found != m_renderObjects.cend() )
            {
                m_renderObjects.erase( found );
            }
        }
    }
} // namespace Ra
