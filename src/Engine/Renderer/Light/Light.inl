#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

inline const Core::Utils::Color& Light::getColor() const {
    return m_color;
}

inline void Light::setColor( const Core::Utils::Color& color ) {
    m_color = color;
}

inline const Light::LightType& Engine::Light::getType() const {
    return m_type;
}

} // namespace Engine
} // namespace Ra
