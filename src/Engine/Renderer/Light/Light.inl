namespace Ra {

inline const Core::Utils::Color& Engine::Light::getColor() const {
    return m_color;
}

inline void Engine::Light::setColor( const Core::Utils::Color& color ) {
    m_color = color;
}

inline const Engine::Light::LightType& Engine::Light::getType() const {
    return m_type;
}

} // namespace Ra
