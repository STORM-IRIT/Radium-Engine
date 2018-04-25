
namespace Ra {

inline void Engine::DirectionalLight::setDirection( const Core::Math::Vector3& dir ) {
    m_direction = dir.normalized();
}

inline const Core::Math::Vector3& Engine::DirectionalLight::getDirection() const {
    return m_direction;
}

} // namespace Ra
