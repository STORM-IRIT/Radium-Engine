#include <Core/File/BlinnPhongMaterialData.hpp>
namespace Ra {
namespace Asset {

inline void BlinnPhongMaterialData::displayInfo() const {
    std::string kd;
    std::string ks;
    std::string ns;
    std::string op;

    if ( m_hasDiffuse )
    {
        Core::StringUtils::stringPrintf( kd, "%.3f %.3f %.3f %.3f", m_diffuse.x(), m_diffuse.y(),
                                         m_diffuse.z(), m_diffuse.w() );
    }

    if ( m_hasSpecular )
    {
        Core::StringUtils::stringPrintf( ks, "%.3f %.3f %.3f %.3f", m_specular.x(),
                                         m_specular.w() );
    }

    if ( m_hasShininess )
    {
        Core::StringUtils::stringPrintf( ns, "%.1f", m_shininess );
    }

    if ( m_hasOpacity )
    {
        Core::StringUtils::stringPrintf( op, "%.15f", m_opacity );
    }

    LOG( logINFO ) << "======== MATERIAL INFO ========";
    LOG( logINFO ) << " Kd             : " << ( m_hasDiffuse ? kd : "NO" );
    LOG( logINFO ) << " Ks             : " << ( m_hasSpecular ? ks : "NO" );
    LOG( logINFO ) << " Ns             : " << ( m_hasShininess ? ns : "NO" );
    LOG( logINFO ) << " Opacity        : " << ( m_hasOpacity ? op : "NO" );
    LOG( logINFO ) << " Kd Texture     : " << ( m_hasTexDiffuse ? m_texDiffuse : "NO" );
    LOG( logINFO ) << " Ks Texture     : " << ( m_hasTexSpecular ? m_texSpecular : "NO" );
    LOG( logINFO ) << " Ns Texture     : " << ( m_hasTexShininess ? m_texShininess : "NO" );
    LOG( logINFO ) << " Normal Texture : " << ( m_hasTexNormal ? m_texNormal : "NO" );
    LOG( logINFO ) << " Alpha Texture  : " << ( m_hasTexOpacity ? m_texOpacity : "NO" );
}

} // namespace Asset
} // namespace Ra
