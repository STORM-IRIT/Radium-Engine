#pragma once

#include <Core/Material/MaterialModel.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Core {
namespace Material {

// RADIUM SUPPORTED MATERIALS
class RA_CORE_API BlinnPhongMaterialModel : public MaterialModel
{
  public:
    explicit BlinnPhongMaterialModel( const std::string& name = "" ) :
        MaterialModel( name, "BlinnPhong" ) {}
    ~BlinnPhongMaterialModel() override = default;

    /// DEBUG
    void displayInfo() const override;

    /// QUERY

    bool hasDiffuseTexture() const { return m_hasTexDiffuse; }

    bool hasSpecularTexture() const { return m_hasTexSpecular; }

    bool hasShininessTexture() const { return m_hasTexShininess; }

    bool hasNormalTexture() const { return m_hasTexNormal; }

    bool hasOpacityTexture() const { return m_hasTexOpacity; }

    /// DATA MEMBERS
    Core::Utils::Color m_kd { 0.7_ra, 0.7_ra, 0.7_ra };
    Core::Utils::Color m_ks { 0.3_ra, 0.3_ra, 0.3_ra };
    Scalar m_ns { 64_ra };
    Scalar m_alpha { 1_ra };
    std::string m_texDiffuse;
    std::string m_texSpecular;
    std::string m_texShininess;
    std::string m_texNormal;
    std::string m_texOpacity;
    bool m_hasTexDiffuse { false };
    bool m_hasTexSpecular { false };
    bool m_hasTexShininess { false };
    bool m_hasTexNormal { false };
    bool m_hasTexOpacity { false };
};

} // namespace Material
} // namespace Core
} // namespace Ra
