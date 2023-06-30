#pragma once

#include <Core/Material/MaterialModel.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Core {
namespace Material {

// RADIUM SUPPORTED MATERIALS
class RA_CORE_API SimpleMaterialModel : public MaterialModel
{
  protected:
    SimpleMaterialModel( const std::string& name, const std::string type ) :
        MaterialModel( name, type ) {}

  public:
    explicit SimpleMaterialModel( const std::string& name = "" ) : MaterialModel( name, "Plain" ) {}
    ~SimpleMaterialModel() override = default;

    /// DEBUG
    void displayInfo() const override;

    /// QUERY
    bool hasDiffuseTexture() const { return m_hasTexDiffuse; }
    bool hasOpacityTexture() const { return m_hasTexOpacity; }

    /// DATA MEMBERS
    Core::Utils::Color m_kd { 0.9_ra, 0.9_ra, 0.9_ra };
    Scalar m_alpha { 1_ra };
    std::string m_texDiffuse;
    std::string m_texOpacity;
    bool m_hasTexDiffuse { false };
    bool m_hasTexOpacity { false };
};

class RA_CORE_API LambertianMaterialModel : public SimpleMaterialModel
{
  public:
    explicit LambertianMaterialModel( const std::string& name = "" ) :
        SimpleMaterialModel( name, "Lambertian" ) {}
    ~LambertianMaterialModel() override = default;

    /// DEBUG
    void displayInfo() const override;

    /// QUERY
    bool hasNormalTexture() const { return m_hasTexNormal; }

    /// DATA MEMBERS
    std::string m_texNormal;
    bool m_hasTexNormal { false };
};

} // namespace Material
} // namespace Core
} // namespace Ra
