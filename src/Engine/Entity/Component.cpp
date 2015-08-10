#include <Engine/Entity/Component.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/System.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace Ra
{

    Engine::Component::Component( const std::string& name )
        : m_name( name )
    {
    }

    Engine::Component::~Component()
    {
    }


} // namespace Ra
