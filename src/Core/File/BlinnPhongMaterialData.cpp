#include <Core/File/BlinnPhongMaterialData.hpp>

namespace Ra {
namespace Asset {

BlinnPhongMaterialData::BlinnPhongMaterialData( const std::string& name ) :
    MaterialData( name, "BlinnPhong" ),
    m_diffuse(),
    m_specular(),
    m_shininess(),
    m_opacity( 1.0 ),
    m_texDiffuse( "" ),
    m_texSpecular( "" ),
    m_texShininess( "" ),
    m_texOpacity( "" ),
    m_texNormal( "" ),
    m_hasDiffuse( false ),
    m_hasSpecular( false ),
    m_hasShininess( false ),
    m_hasOpacity( false ),
    m_hasTexDiffuse( false ),
    m_hasTexSpecular( false ),
    m_hasTexShininess( false ),
    m_hasTexOpacity( false ),
    m_hasTexNormal( false ) {}

} // namespace Asset
} // namespace Ra
