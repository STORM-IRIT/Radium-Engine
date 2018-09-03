#include <Engine/Renderer/Light/SpotLight.hpp>

namespace Ra {
namespace Engine {

inline void SpotLight::setPosition( const Core::Vector3& position ) {
    m_position = position;
}

inline const Core::Vector3& SpotLight::getPosition() const {
    return m_position;
}

inline void SpotLight::setDirection( const Core::Vector3& direction ) {
    m_direction = direction.normalized();
}

inline const Core::Vector3& SpotLight::getDirection() const {
    return m_direction;
}

inline void SpotLight::setInnerAngleInRadians( Scalar angle ) {
    m_innerAngle = angle;
}

inline void SpotLight::setInnerAngleInDegrees( Scalar angle ) {
    m_innerAngle = angle * Core::Math::toRad;
}

inline Scalar SpotLight::getInnerAngle() const {
    return m_innerAngle;
}

inline void SpotLight::setOuterAngleInRadians( Scalar angle ) {
    m_outerAngle = angle;
}

inline void SpotLight::setOuterAngleInDegrees( Scalar angle ) {
    m_outerAngle = Core::Math::toRad * angle;
}

inline Scalar SpotLight::getOuterAngle() const {
    return m_outerAngle;
}

inline void SpotLight::setAttenuation( const Attenuation& attenuation ) {
    m_attenuation = attenuation;
}

inline void SpotLight::setAttenuation( Scalar constant, Scalar linear, Scalar quadratic ) {
    m_attenuation.constant = constant;
    m_attenuation.linear = linear;
    m_attenuation.quadratic = quadratic;
}

inline const SpotLight::Attenuation& SpotLight::getAttenuation() const {
    return m_attenuation;
}

} // namespace Engine
} // namespace Ra
