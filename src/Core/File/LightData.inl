#include <Core/File/LightData.hpp>

#include "LightData.hpp"
#include <Core/Log/Log.hpp>

namespace Ra {
namespace Asset {

/////////////////////
///  LIGHT DATA   ///
/////////////////////

/// NAME
inline void LightData::setName( const std::string& name ) {
    m_name = name;
}

/// TYPE
inline LightData::LightType LightData::getType() const {
    return m_type;
}

inline void LightData::setType( const LightType& type ) {
    m_type = type;
}

/// FRAME
inline const Core::Matrix4& LightData::getFrame() const {
    return m_frame;
}

inline void LightData::setFrame( const Core::Matrix4& frame ) {
    m_frame = frame;
}

/// construct a directional light
inline void LightData::setLight( const Core::Color& color, const Core::Vector3& direction ) {
    m_type = DIRECTIONAL_LIGHT;
    m_color = color;
    m_dirlight.direction = direction;
}

/// construct a point light
inline void LightData::setLight( const Core::Color& color, const Core::Vector3& position,
                                 LightAttenuation attenuation ) {
    m_type = POINT_LIGHT;
    m_color = color;
    m_pointlight.position = position;
    m_pointlight.attenuation = attenuation;
}

/// construct a spot light
inline void LightData::setLight( const Core::Color& color, const Core::Vector3& position,
                                 const Core::Vector3& direction, Scalar inAngle, Scalar outAngle,
                                 LightAttenuation attenuation ) {
    m_type = SPOT_LIGHT;
    m_color = color;
    m_spotlight.position = position;
    m_spotlight.direction = direction;
    m_spotlight.innerAngle = inAngle;
    m_spotlight.outerAngle = outAngle;
    m_spotlight.attenuation = attenuation;
}

/// construct an area light
inline void LightData::setLight( const Core::Color& color, const Core::Vector3& cog,
                                 const Core::Matrix3& spatialCov, const Core::Matrix3& normalCov,
                                 LightAttenuation attenuation ) {
    m_type = AREA_LIGHT;
    m_color = color;
    m_arealight.position = cog;
    m_arealight.spatialCovariance = spatialCov;
    m_arealight.normalCovariance = normalCov;
    m_arealight.attenuation = attenuation;
}

/// QUERY
inline bool LightData::isPointLight() const {
    return ( m_type == POINT_LIGHT );
}

inline bool LightData::isSpotLight() const {
    return ( m_type == SPOT_LIGHT );
}

inline bool LightData::isDirectionalLight() const {
    return ( m_type == DIRECTIONAL_LIGHT );
}

inline bool LightData::isAreaLight() const {
    return ( m_type == AREA_LIGHT );
}

/// DEBUG
inline void LightData::displayInfo() const {
    std::string type;
    switch ( m_type )
    {
    case UNKNOWN:
        type = "UNKNOWN";
        break;
    case POINT_LIGHT:
        type = "POINT LIGHT";
        break;
    case SPOT_LIGHT:
        type = "SPOT LIGHT";
        break;
    case DIRECTIONAL_LIGHT:
        type = "DIRECTIONAL LIGHT";
        break;
    case AREA_LIGHT:
        type = "AREA LIGHT";
        break;
    }
    LOG( logINFO ) << "======== LIGHT INFO ========";
    LOG( logINFO ) << " Name           : " << m_name;
    LOG( logINFO ) << " Type           : " << type;
}

} // namespace Asset
} // namespace Ra
