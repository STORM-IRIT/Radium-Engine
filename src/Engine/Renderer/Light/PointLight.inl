

namespace Ra {

inline void Engine::PointLight::setPosition( const Core::Vector3& pos ) {
    m_position = pos;
}

inline const Core::Vector3& Engine::PointLight::getPosition() const {
    return m_position;
}

inline void Engine::PointLight::setAttenuation( const PointLight::Attenuation& attenuation ) {
    m_attenuation = attenuation;
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
