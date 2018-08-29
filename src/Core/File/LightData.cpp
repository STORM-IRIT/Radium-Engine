#include <Core/File/LightData.hpp>

namespace Ra {
namespace Asset {

LightData::LightData( const std::string& name, const LightType& type ) :
    AssetData( name ),
    m_frame( Core::Matrix4::Identity() ),
    m_type( type ) {}

LightData::~LightData() {}

} // namespace Asset
} // namespace Ra
