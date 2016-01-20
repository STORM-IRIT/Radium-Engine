#include <Engine/Entity/Component.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/System.hpp>
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
            for (const auto& ro : renderObjects )
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
            renderObjects.push_back( getRoMgr()->addRenderObject( renderObject ) );
            return renderObjects.back();
        }

        void Component::removeRenderObject( Core::Index roIdx )
        {
            auto found = std::find(renderObjects.cbegin(), renderObjects.cend(),roIdx);
            CORE_WARN_IF(found == renderObjects.cbegin(), " Render object not found in component");
            if (found != renderObjects.cend() && getRoMgr() )
            {
                getRoMgr()->removeRenderObject(*found);
                renderObjects.erase(found);
            }
        }

        Core::Transform Component::getWorldTransform() const
        {
           return getEntity()->getTransform();
        }

    }
} // namespace Ra
