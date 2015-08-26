#include <Engine/Entity/Component.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/System.hpp>

#include <Engine/Renderer/RenderObject/Gizmo/Gizmo.hpp>

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
} // namespace Ra
