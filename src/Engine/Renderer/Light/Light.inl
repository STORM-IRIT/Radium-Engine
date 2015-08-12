#include <Engine/Renderer/Light/Light.hpp>

namespace Ra
{

    inline const Core::Color& Engine::Light::getColor() const
    {
        return m_color;
    }

    inline void Engine::Light::setColor( const Core::Color& color )
    {
        m_color = color;
    }

    inline const Engine::Light::LightType& Engine::Light::getType() const
    {
        return m_type;
    }

} // namespace Ra
