#include <Engine/Renderer/Light/DirLight.hpp>

namespace Ra {
namespace Engine {

inline void DirectionalLight::setDirection( const Core::Vector3& dir ) {
    m_direction = dir.normalized();
}

inline const Core::Vector3& DirectionalLight::getDirection() const {
    return m_direction;
}

} // namespace Engine
} // namespace Ra
