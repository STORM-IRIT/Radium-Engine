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
            for (const auto& ro : m_renderObjects )
            {
                getRoMgr()->removeRenderObject( ro );
            }
        }

        RenderObjectManager* Component::getRoMgr()
        {
            return RadiumEngine::getInstance()->getRenderObjectManager();
        }

        void Component::addRenderObject( RenderObject* renderObject )
        {
            m_renderObjects.push_back( getRoMgr()->addRenderObject( renderObject ) );
        }
    }
} // namespace Ra
