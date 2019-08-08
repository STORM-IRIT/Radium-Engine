

namespace Ra::Engine {

inline void PointLight::setPosition( const Eigen::Matrix<Scalar, 3, 1>& pos ) {
    m_position = pos;
}

inline const Eigen::Matrix<Scalar, 3, 1>& PointLight::getPosition() const {
    return m_position;
}

inline void PointLight::setAttenuation( const PointLight::Attenuation& attenuation ) {
    m_attenuation = attenuation;
}

inline void PointLight::setAttenuation( Scalar constant, Scalar linear, Scalar quadratic ) {
    m_attenuation.constant  = constant;
    m_attenuation.linear    = linear;
    m_attenuation.quadratic = quadratic;
}

inline const PointLight::Attenuation& PointLight::getAttenuation() const {
    return m_attenuation;
}

} // namespace Ra
