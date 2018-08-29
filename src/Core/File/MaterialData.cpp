#include <Core/File/MaterialData.hpp>

namespace Ra {
namespace Asset {

MaterialData::MaterialData( const std::string& name, const std::string& type ) :
    AssetData( name ),
    m_type( type ) {}

MaterialData::~MaterialData() {}

void MaterialData::displayInfo() const {
    LOG( logERROR ) << "MaterialData : unkonwn material type : " << m_type;
}

} // namespace Asset
} // namespace Ra
