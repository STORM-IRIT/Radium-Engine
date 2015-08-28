#include <Engine/Entity/Component.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/System.hpp>

namespace Ra
{

    Engine::Component::Component( const std::string& name )
        : m_name( name ), m_entity(nullptr)
    {
    }

    Engine::Component::~Component()
    {
    }

    Engine::RenderObjectManager* Engine::Component::getRoMgr()
    {
        return RadiumEngine::getInstance()->getRenderObjectManager();
    }

    void Engine::Component::addDrawable(Engine::RenderObject* ro) const
    {
        getRoMgr()->addRenderObject(ro);
    }
} // namespace Ra