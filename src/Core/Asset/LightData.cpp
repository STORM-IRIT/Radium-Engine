#include <Core/Asset/LightData.hpp>

#if 0
// this will add a dependence on the core to the engine. Not a good idea ...
#    include <Engine/Renderer/Light/DirLight.hpp>
#    include <Engine/Renderer/Light/Light.hpp>
#    include <Engine/Renderer/Light/PointLight.hpp>
#    include <Engine/Renderer/Light/SpotLight.hpp>
#endif

namespace Ra {
namespace Core {
namespace Asset {

/// CONSTRUCTOR
LightData::LightData( const std::string& name, const LightType& type ) :
    AssetData( name ),
    m_frame( Core::Math::Matrix4::Identity() ),
    m_type( type ) {}

/// DESTRUCTOR
LightData::~LightData() {}

#if 0
// this will add a dependence on the core to the engine. Not a good idea ...
Engine::Light* LightData::getLight() const {
    switch (getType()) {
    case DIRECTIONAL_LIGHT:
    {
        auto thelight = new Engine::DirectionalLight(m_name);
        thelight->setColor( m_color );
        thelight->setDirection( m_dirlight.direction );
        return thelight;
    }
    case POINT_LIGHT:
    {
        auto thelight = new Engine::PointLight(m_name);
        thelight->setColor(m_color);
        thelight->setPosition(m_pointlight.position);
        thelight->setAttenuation(m_pointlight.attenuation.constant, m_pointlight.attenuation.linear, m_pointlight.attenuation.quadratic);
        return thelight;
    }
    case SPOT_LIGHT:
    {
        auto thelight = new Engine::SpotLight(m_name);
        thelight->setColor(m_color);
        thelight->setPosition(m_spotlight.position);
        thelight->setDirection(m_spotlight.direction);
        thelight->setAttenuation(m_spotlight.attenuation.constant, m_spotlight.attenuation.linear, m_spotlight.attenuation.quadratic);
        thelight->setInnerAngleInRadians(m_spotlight.innerAngle);
        thelight->setOuterAngleInRadians(m_spotlight.outerAngle);
        return thelight;
    }
    case AREA_LIGHT:
    {
        // No arealight for now
        return nullptr;
    }
    default:
        return nullptr;
    }
}

#endif

} // namespace Asset
} // namespace Core
} // namespace Ra
