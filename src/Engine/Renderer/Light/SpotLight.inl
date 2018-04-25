
namespace Ra {

inline void Engine::SpotLight::setPosition( const Core::Math::Vector3& position ) {
    m_position = position;
}

inline const Core::Math::Vector3& Engine::SpotLight::getPosition() const {
    return m_position;
}

inline void Engine::SpotLight::setDirection( const Core::Math::Vector3& direction ) {
    m_direction = direction.normalized();
}

inline const Core::Math::Vector3& Engine::SpotLight::getDirection() const {
    return m_direction;
}

inline void Engine::SpotLight::setInnerAngleInRadians( Scalar angle ) {
    m_innerAngle = angle;
}

inline void Engine::SpotLight::setOuterAngleInRadians( Scalar angle ) {
    m_outerAngle = angle;
}

inline void Engine::SpotLight::setInnerAngleInDegrees( Scalar angle ) {
    m_innerAngle = angle * Core::Math::toRad;
}

inline void Engine::SpotLight::setOuterAngleInDegrees( Scalar angle ) {
    m_outerAngle = Core::Math::toRad * angle;
}

inline Scalar Engine::SpotLight::getInnerAngle() const {
    return m_innerAngle;
}

inline Scalar Engine::SpotLight::getOuterAngle() const {
    return m_outerAngle;
}

inline void Engine::SpotLight::setAttenuation( const Attenuation& attenuation ) {
    m_attenuation = attenuation;
}

inline void Engine::SpotLight::setAttenuation( Scalar constant, Scalar linear, Scalar quadratic ) {
    m_attenuation.constant = constant;
    m_attenuation.linear = linear;
    m_attenuation.quadratic = quadratic;
}

inline const Engine::SpotLight::Attenuation& Engine::SpotLight::getAttenuation() const {
    return m_attenuation;
}

} // namespace Ra
