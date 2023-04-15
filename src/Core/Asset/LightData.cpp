#include <Core/Asset/LightData.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// CONSTRUCTOR

LightData::LightData( const LightData& data ) :
    AssetData( data ), m_frame( data.m_frame ), m_type( data.m_type ), m_color( data.m_color ) {

    switch ( m_type ) {
    case POINT_LIGHT:
        m_pointlight = data.m_pointlight;
        break;
    case SPOT_LIGHT:
        m_spotlight = data.m_spotlight;
        break;
    case DIRECTIONAL_LIGHT:
        m_dirlight = data.m_dirlight;
        break;
    case AREA_LIGHT:
        m_arealight = data.m_arealight;
        break;
    default:
        break;
    }
}

void LightData::displayInfo() const {
    using namespace Core::Utils; // log
    std::string type;
    switch ( m_type ) {
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
    case UNKNOWN:
    default:
        type = "UNKNOWN";
        break;
    }
    LOG( logINFO ) << "======== LIGHT INFO ========";
    LOG( logINFO ) << " Name           : " << getName();
    LOG( logINFO ) << " Type           : " << type;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
