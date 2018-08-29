#ifndef RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP
#define RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP

#include <Core/File/MaterialData.hpp>

namespace Ra {
namespace Asset {

// RADIUM SUPPORTED MATERIALS
class RA_CORE_API BlinnPhongMaterialData : public MaterialData {
  public:
    explicit BlinnPhongMaterialData( const std::string& name = "" );

    /// Print stat info to the Debug output.
    inline void displayInfo() const final;

    /// The diffuse color.
    Core::Color m_diffuse;

    /// The specular color.
    Core::Color m_specular;

    /// The shininess exponent.
    Scalar m_shininess;

    /// The alpha value.
    Scalar m_opacity;

    /// The diffuse texture filename.
    std::string m_texDiffuse;

    /// The specular texture filename.
    std::string m_texSpecular;

    /// The shininess exponent filename.
    std::string m_texShininess;

    /// The alpha texture filename.
    std::string m_texOpacity;

    /// The normal texture filename.
    std::string m_texNormal;

    /// Whether the Material has a Diffuse color.
    bool m_hasDiffuse;

    /// Whether the Material has a Specular color.
    bool m_hasSpecular;

    /// Whether the Material has a Shinines exponent.
    bool m_hasShininess;

    /// Whether the Material has an Alpha value.
    bool m_hasOpacity;

    /// Whether the Material has a Diffuse texture.
    bool m_hasTexDiffuse;

    /// Whether the Material has a Specular texture.
    bool m_hasTexSpecular;

    /// Whether the Material has a Shininess texture.
    bool m_hasTexShininess;

    /// Whether the Material has an Alpha texture.
    bool m_hasTexOpacity;

    /// Whether the Material has a Normal texture.
    bool m_hasTexNormal;
};

} // namespace Asset
} // namespace Ra

#include <Core/File/BlinnPhongMaterialData.inl>

#endif // RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP
