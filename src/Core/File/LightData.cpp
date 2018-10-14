#include <Core/File/LightData.hpp>

#if 0
// this will add a dependence on the core to the engine. Not a good idea ...
#    include <Engine/Renderer/Light/DirLight.hpp>
#    include <Engine/Renderer/Light/Light.hpp>
#    include <Engine/Renderer/Light/PointLight.hpp>
#    include <Engine/Renderer/Light/SpotLight.hpp>
#endif

namespace Ra {
namespace Asset {

/// CONSTRUCTOR
LightData::LightData( const std::string& name, const LightType& type ) :
    AssetData( name ),
    m_frame( Core::Matrix4::Identity() ),
    m_type( type ) {}

LightData::LightData( const LightData& data ) : AssetData(data), m_frame(data.m_frame), m_type(data.m_type), m_color(data.m_color) {

    switch (m_type) {
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

/// DESTRUCTOR
LightData::~LightData() {}


} // namespace Asset
} // namespace Ra
