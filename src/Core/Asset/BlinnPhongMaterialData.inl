#include <Core/Asset/BlinnPhongMaterialData.hpp>
namespace Ra {
namespace Core {
namespace Asset {

///////////////////
/// BLINN PHONG ///
///////////////////
inline bool BlinnPhongMaterialData::hasDiffuse() const {
    return m_hasDiffuse;
}

inline bool BlinnPhongMaterialData::hasSpecular() const {
    return m_hasSpecular;
}

inline bool BlinnPhongMaterialData::hasShininess() const {
    return m_hasShininess;
}

inline bool BlinnPhongMaterialData::hasOpacity() const {
    return m_hasOpacity;
}

inline bool BlinnPhongMaterialData::hasDiffuseTexture() const {
    return m_hasTexDiffuse;
}

inline bool BlinnPhongMaterialData::hasSpecularTexture() const {
    return m_hasTexSpecular;
}

inline bool BlinnPhongMaterialData::hasShininessTexture() const {
    return m_hasTexShininess;
}

inline bool BlinnPhongMaterialData::hasNormalTexture() const {
    return m_hasTexNormal;
}

inline bool BlinnPhongMaterialData::hasOpacityTexture() const {
    return m_hasTexOpacity;
}

/// DEBUG
inline void BlinnPhongMaterialData::displayInfo() const {
    std::string kd;
    std::string ks;
    std::string ns;
    std::string op;

    if ( hasDiffuse() )
    {
        Core::Utils::stringPrintf( kd, "%.3f %.3f %.3f %.3f", m_diffuse.x(), m_diffuse.y(),
                                         m_diffuse.z(), m_diffuse.w() );
    }

    if ( hasSpecular() )
    {
        Core::Utils::stringPrintf( ks, "%.3f %.3f %.3f %.3f", m_specular.x(),
                                         m_specular.w() );
    }

    if ( hasShininess() )
    {
        Core::Utils::stringPrintf( ns, "%.1f", m_shininess );
    }

    if ( hasOpacity() )
    {
        Core::Utils::stringPrintf( op, "%.15f", m_opacity );
    }

    LOG( Core::Utils::logINFO ) << "======== MATERIAL INFO ========";
    LOG( Core::Utils::logINFO ) << " Kd             : " << ( hasDiffuse() ? kd : "NO" );
    LOG( Core::Utils::logINFO ) << " Ks             : " << ( hasSpecular() ? ks : "NO" );
    LOG( Core::Utils::logINFO ) << " Ns             : " << ( hasShininess() ? ns : "NO" );
    LOG( Core::Utils::logINFO ) << " Opacity        : " << ( hasOpacity() ? op : "NO" );
    LOG( Core::Utils::logINFO ) << " Kd Texture     : " << ( hasDiffuseTexture() ? m_texDiffuse : "NO" );
    LOG( Core::Utils::logINFO ) << " Ks Texture     : " << ( hasSpecularTexture() ? m_texSpecular : "NO" );
    LOG( Core::Utils::logINFO ) << " Ns Texture     : " << ( hasShininessTexture() ? m_texShininess : "NO" );
    LOG( Core::Utils::logINFO ) << " Normal Texture : " << ( hasNormalTexture() ? m_texNormal : "NO" );
    LOG( Core::Utils::logINFO ) << " Alpha Texture  : " << ( hasOpacityTexture() ? m_texOpacity : "NO" );
}


} // namespace Asset
} // namespace Core
} // namespace Ra
