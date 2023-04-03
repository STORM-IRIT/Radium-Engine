#pragma once

#include <Core/Asset/MaterialData.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Core {
namespace Asset {

// RADIUM SUPPORTED MATERIALS
class RA_CORE_API BlinnPhongMaterialData : public MaterialData
{
  public:
    explicit BlinnPhongMaterialData( const std::string& name = "" );

    /// DEBUG
    inline void displayInfo() const final;

    /// QUERY
    inline bool hasDiffuse() const;

    inline bool hasSpecular() const;

    inline bool hasShininess() const;

    inline bool hasOpacity() const;

    inline bool hasDiffuseTexture() const;

    inline bool hasSpecularTexture() const;

    inline bool hasShininessTexture() const;

    inline bool hasNormalTexture() const;

    inline bool hasOpacityTexture() const;

    /// DATA MEMBERS
    Core::Utils::Color m_diffuse;
    Core::Utils::Color m_specular;
    Scalar m_shininess;
    Scalar m_opacity;
    std::string m_texDiffuse;
    std::string m_texSpecular;
    std::string m_texShininess;
    std::string m_texNormal;
    std::string m_texOpacity;
    bool m_hasDiffuse;
    bool m_hasSpecular;
    bool m_hasShininess;
    bool m_hasOpacity;
    bool m_hasTexDiffuse;
    bool m_hasTexSpecular;
    bool m_hasTexShininess;
    bool m_hasTexNormal;
    bool m_hasTexOpacity;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Utils/Log.hpp>

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
    using namespace Core::Utils; // log
    auto print = []( bool ok, const std::string& name, const auto& value ) {
        if ( ok ) { LOG( logINFO ) << name << value; }
        else {
            LOG( logINFO ) << name << "NO";
        }
    };

    LOG( logINFO ) << "======== MATERIAL INFO ========";
    print( hasDiffuse(), " Kd             : ", m_diffuse.transpose() );
    print( hasSpecular(), " Ks             : ", m_specular.transpose() );
    print( hasShininess(), " Ns             : ", m_shininess );
    print( hasOpacity(), " Opacity        : ", m_opacity );
    print( hasDiffuseTexture(), " Kd Texture     : ", m_texDiffuse );
    print( hasSpecularTexture(), " Ks Texture     : ", m_texSpecular );
    print( hasShininessTexture(), " Ns Texture     : ", m_texShininess );
    print( hasNormalTexture(), " Normal Texture : ", m_texNormal );
    print( hasOpacityTexture(), " Alpha Texture  : ", m_texOpacity );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
