namespace Ra
{

inline void Engine::DirectionalLight::setDirection(const Core::Vector3& dir)
{
    m_direction = dir;
}

inline const Core::Vector3& Engine::DirectionalLight::getDirection() const
{
    return m_direction;
}

}
