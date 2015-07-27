#include <Engine/Entity/Component.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/System.hpp>
#include <Engine/Renderer/Drawable/DrawableManager.hpp>

namespace Ra
{

Engine::Component::Component(const std::string& name )
    : m_name(name)
{
}

Engine::Component::~Component()
{
}


} // namespace Ra
