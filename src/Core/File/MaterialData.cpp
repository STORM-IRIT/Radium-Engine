#include <Core/File/MaterialData.hpp>

namespace Ra {
  namespace Asset {


/// CONSTRUCTOR
    MaterialData::MaterialData(const std::string&  name, const MaterialType& type )
        : AssetData( name )
        , m_type(type)
        , m_BlinnPhong()
    {

    }

    MaterialData::MaterialData( const MaterialData& material ) : AssetData(material.m_name), m_type( material.m_type )
    {
        switch ( m_type )
        {
            case BLINN_PHONG:
                m_BlinnPhong = material.m_BlinnPhong;
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

    MaterialData& MaterialData::operator=( const MaterialData& material )
    {
        if (this != &material)
        {
            m_name = material.m_name;
            m_type = material.m_type;
            switch ( m_type )
            {
                case BLINN_PHONG:
                    m_BlinnPhong = material.m_BlinnPhong;
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
        return *this;
    }

    ///////////////////
    /// BLINN PHONG ///
    ///////////////////

    MaterialData::BlinnPhongMaterial::BlinnPhongMaterial()
        : m_diffuse()
        , m_specular()
        , m_shininess()
        , m_opacity(1.0)
        , m_texDiffuse("")
        , m_texSpecular("")
        , m_texShininess("")
        , m_texNormal("")
        , m_texOpacity("")
        , m_hasDiffuse( false )
        , m_hasSpecular( false )
        , m_hasShininess( false )
        , m_hasOpacity(false)
        , m_hasTexDiffuse(false)
        , m_hasTexSpecular(false)
        , m_hasTexShininess(false)
        , m_hasTexNormal(false)
        , m_hasTexOpacity(false)
    {

    }

  } // namespace Asset
} // namespace Ra
