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

///////////////////
/// BLINN PHONG ///
///////////////////
BlinnPhongMaterialData::BlinnPhongMaterialData( const std::string& name ) :
    MaterialData( name, "BlinnPhong" ),
    m_diffuse(),
    m_specular(),
    m_shininess(),
    m_opacity( 1.0 ),
    m_texDiffuse( "" ),
    m_texSpecular( "" ),
    m_texShininess( "" ),
    m_texNormal( "" ),
    m_texOpacity( "" ),
    m_hasDiffuse( false ),
    m_hasSpecular( false ),
    m_hasShininess( false ),
    m_hasOpacity( false ),
    m_hasTexDiffuse( false ),
    m_hasTexSpecular( false ),
    m_hasTexShininess( false ),
    m_hasTexNormal( false ),
    m_hasTexOpacity( false ) {}

BlinnPhongMaterialData::~BlinnPhongMaterialData() {}

} // namespace Asset
} // namespace Ra
