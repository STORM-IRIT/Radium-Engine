#ifndef RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP
#define RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP

#include <Core/Asset/MaterialData.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * The BlinnPhongMaterialData class stores all the parameters of Blinn-Phong materials.
 */
class RA_CORE_API BlinnPhongMaterialData : public MaterialData {
  public:
    explicit BlinnPhongMaterialData( const std::string& name = "" );

    ~BlinnPhongMaterialData() override = default;

    /**
     * Print stat info to the Debug output.
     */
    inline void displayInfo() const override final;

    /**
     * Returns true if the material has a diffuse color, false otherwise.
     */
    inline bool hasDiffuse() const;

    /**
     * Returns true if the material has a specular color, false otherwise.
     */
    inline bool hasSpecular() const;

    /**
     * Returns true if the material has a shininess value, false otherwise.
     */
    inline bool hasShininess() const;

    /**
     * Returns true if the material has an opacity value, false otherwise.
     */
    inline bool hasOpacity() const;

    /**
     * Returns true if the material has a diffuse texture, false otherwise.
     */
    inline bool hasDiffuseTexture() const;

    /**
     * Returns true if the material has a specular texture, false otherwise.
     */
    inline bool hasSpecularTexture() const;

    /**
     * Returns true if the material has a shininess texture, false otherwise.
     */
    inline bool hasShininessTexture() const;

    /**
     * Returns true if the material has a normal texture, false otherwise.
     */
    inline bool hasNormalTexture() const;

    /**
     * Returns true if the material has an opacity texture, false otherwise.
     */
    inline bool hasOpacityTexture() const;

    /// The diffuse color.
    Core::Utils::Color m_diffuse;

    /// The specular color.
    Core::Utils::Color m_specular;

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
    std::string m_texNormal;

    /// The normal texture filename.
    std::string m_texOpacity;

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
    bool m_hasTexNormal;

    /// Whether the Material has a Normal texture.
    bool m_hasTexOpacity;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/BlinnPhongMaterialData.inl>

#endif // RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP
