#include <Core/Asset/MaterialData.hpp>
#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// CONSTRUCTOR
MaterialData::MaterialData( const std::string& name, const std::string& type ) :
    AssetData( name ), m_type( type ) {}

MaterialData::~MaterialData() {}

/// DEBUG
void MaterialData::displayInfo() const {
    using namespace Core::Utils; // log
    LOG( logERROR ) << "MaterialData : unkonwn material type : " << m_type;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
