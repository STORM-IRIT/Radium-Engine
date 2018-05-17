

namespace Ra {

inline void Engine::PointLight::setPosition( const Core::Math::Vector3& pos ) {
    m_position = pos;
}

inline const Core::Math::Vector3& Engine::PointLight::getPosition() const {
    return m_position;
}

inline void Engine::PointLight::setAttenuation( const PointLight::Attenuation& att ) {
    m_attenuation = att;
}

inline void Engine::PointLight::setAttenuation( Scalar constant, Scalar linear, Scalar quadratic ) {
    m_attenuation.constant = constant;
    m_attenuation.linear = linear;
    m_attenuation.quadratic = quadratic;
}

inline const Engine::PointLight::Attenuation& Engine::PointLight::getAttenuation() const {
    return m_attenuation;
}

} // namespace Ra
