#pragma once

#include "Light.hpp"

namespace Ra {
namespace Engine {
namespace Scene {

inline const Core::Utils::Color& Light::getColor() const {
    return m_color;
}

inline void Light::setColor( const Core::Utils::Color& color ) {
    m_color = color;
}

inline const Light::LightType& Light::getType() const {
    return m_type;
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
