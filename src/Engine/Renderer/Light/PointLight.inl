#include <Engine/Renderer/Light/PointLight.hpp>

namespace Ra {
namespace Engine {

inline void PointLight::setPosition( const Core::Vector3& pos ) {
    m_position = pos;
}

inline const Core::Vector3& PointLight::getPosition() const {
    return m_position;
}

inline void PointLight::setAttenuation( const PointLight::Attenuation& att ) {
    m_attenuation = att;
}

inline void PointLight::setAttenuation( Scalar constant, Scalar linear, Scalar quadratic ) {
    m_attenuation.constant = constant;
    m_attenuation.linear = linear;
    m_attenuation.quadratic = quadratic;
}

inline const PointLight::Attenuation& PointLight::getAttenuation() const {
    return m_attenuation;
}

} // namespace Engine
} // namespace Ra
