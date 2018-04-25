#include <Core/File/MaterialData.hpp>

namespace Ra {
namespace Asset {

/// CONSTRUCTOR
MaterialData::MaterialData( const std::string& name, const std::string& type ) :
    AssetData( name ),
    m_type( type ) {}

MaterialData::~MaterialData() {}

/// DEBUG
void MaterialData::displayInfo() const {
    LOG( logERROR ) << "MaterialData : unkonwn material type : " << m_type;
}

} // namespace Asset
} // namespace Ra
