#include <Core/File/MaterialData.hpp>

#include <Core/Log/Log.hpp>

#include <iterator>
#include <algorithm>

namespace Ra {
  namespace Asset {


    ////////////////
    /// MATERIAL ///
    ////////////////

    inline MaterialData::~MaterialData(){

    }

    /// NAME
    inline void MaterialData::setName( const std::string& name )
    {
        m_name = name;
    }

    /// TYPE
    inline MaterialData::MaterialType MaterialData::getType() const
    {
        return m_type;
    }

    inline void MaterialData::setType( const MaterialType& type )
    {
        m_type = type;
    }

    /// DEBUG
    inline void MaterialData::displayInfo() const
    {
        switch ( m_type ) {
            case BLINN_PHONG:
                m_BlinnPhong.displayInfo();
                break;
            case DISNEY:
            case MATTE:
            case METAL:
            case MIRROR:
            case PLASTIC:
            case SUBSTRATE:
            case TRANSLUCENT:
            case UNKNOWN:
                LOG (logERROR) << "MaterialData : unkonwn material type";
        }
    }

    /// QUERY
    inline const MaterialData::BlinnPhongMaterial &MaterialData::getBlinnPhong() const
    {
        return m_BlinnPhong;
    }

    inline void MaterialData::setBlinnPhong(const MaterialData::BlinnPhongMaterial &o)
    {
        m_BlinnPhong = o;
    }

    ///////////////////
    /// BLINN PHONG ///
    ///////////////////
    inline bool MaterialData::BlinnPhongMaterial::hasDiffuse() const
    {
        return m_hasDiffuse;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasSpecular() const
    {
        return m_hasSpecular;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasShininess() const
    {
        return m_hasShininess;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasOpacity() const
    {
        return m_hasOpacity;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasDiffuseTexture() const
    {
        return m_hasTexDiffuse;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasSpecularTexture() const
    {
        return m_hasTexSpecular;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasShininessTexture() const
    {
        return m_hasTexShininess;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasNormalTexture() const
    {
        return m_hasTexNormal;
    }

    inline bool MaterialData::BlinnPhongMaterial::hasOpacityTexture() const
    {
        return m_hasTexOpacity;
    }

    /// DEBUG
    inline void MaterialData::BlinnPhongMaterial::displayInfo() const
    {
        std::string kd;
        std::string ks;
        std::string ns;
        std::string op;

        if( hasDiffuse() ) {
            Core::StringUtils::stringPrintf( kd, "%.3f %.3f %.3f %.3f",
                                             m_diffuse.x(),
                                             m_diffuse.y(),
                                             m_diffuse.z(),
                                             m_diffuse.w() );
        }

        if( hasSpecular() )
        {
            Core::StringUtils::stringPrintf( ks, "%.3f %.3f %.3f %.3f",
                                             m_specular.x(),
                                             m_specular.w() );
        }

        if( hasShininess() )
        {
            Core::StringUtils::stringPrintf( ns, "%.1f", m_shininess );
        }

        if (hasOpacity())
        {
            Core::StringUtils::stringPrintf(op, "%.15f", m_opacity);
        }

        LOG( logINFO ) << "======== MATERIAL INFO ========";
        LOG( logINFO ) << " Kd             : " << ( hasDiffuse()          ? kd             : "NO" );
        LOG( logINFO ) << " Ks             : " << ( hasSpecular()         ? ks             : "NO" );
        LOG( logINFO ) << " Ns             : " << ( hasShininess()        ? ns             : "NO" );
        LOG( logINFO ) << " Opacity        : " << ( hasOpacity()          ? op             : "NO" );
        LOG( logINFO ) << " Kd Texture     : " << ( hasDiffuseTexture()   ? m_texDiffuse   : "NO" );
        LOG( logINFO ) << " Ks Texture     : " << ( hasSpecularTexture()  ? m_texSpecular  : "NO" );
        LOG( logINFO ) << " Ns Texture     : " << ( hasShininessTexture() ? m_texShininess : "NO" );
        LOG( logINFO ) << " Normal Texture : " << ( hasNormalTexture()    ? m_texNormal    : "NO" );
        LOG( logINFO ) << " Alpha Texture  : " << ( hasOpacityTexture()   ? m_texOpacity   : "NO" );
    }

  } // namespace Asset
} // namespace Ra
