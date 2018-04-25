#ifndef RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP
#define RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP

#include <Core/File/MaterialData.hpp>

namespace Ra {
namespace Asset {

// RADIUM SUPPORTED MATERIALS
  class RA_CORE_API BlinnPhongMaterialData : public MaterialData {
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
  Core::Color m_diffuse;
  Core::Color m_specular;
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
} // namespace Ra

#include <Core/File/BlinnPhongMaterialData.inl>

#endif // RADIUMENGINE_BLINNPHONGMATERIALDATA_HPP
