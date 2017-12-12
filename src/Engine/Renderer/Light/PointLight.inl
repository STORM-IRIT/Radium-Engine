#include <Engine/Renderer/Light/PointLight.hpp>

namespace Ra
{
    
    inline void Engine::PointLight::setPosition( const Core::Vector3& pos )
    {
        m_position = pos;
    }
    
    inline const Core::Vector3& Engine::PointLight::getPosition() const
    {
        return m_position;
    }
    
    inline void Engine::PointLight::setAttenuation( const PointLight::Attenuation& att )
    {
        m_attenuation = att;
    }
    
    inline void Engine::PointLight::setAttenuation( Scalar c, Scalar l, Scalar q )
    {
        m_attenuation.constant = c;
        m_attenuation.linear = l;
        m_attenuation.quadratic = q;
    }
    
    inline const Engine::PointLight::Attenuation& Engine::PointLight::getAttenuation() const
    {
        return m_attenuation;
    }
    
} // namespace Ra
