#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

inline const Core::Color& Light::getColor() const {
    return m_color;
}

inline void Light::setColor( const Core::Color& color ) {
    m_color = color;
}

inline const Light::LightType& Light::getType() const {
    return m_type;
}

} // namespace Engine
} // namespace Ra
