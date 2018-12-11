#ifndef RADIUMENGINE_POINTLIGHT_HPP
#define RADIUMENGINE_POINTLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

  /** Point light for rendering
   *
   */
class RA_ENGINE_API PointLight final : public Light {
  public:
    RA_CORE_ALIGNED_NEW

    explicit PointLight( Entity* entity, const std::string& name = "pointlight" );
    ~PointLight() override = default;

    void getRenderParameters( RenderParameters& params ) const override;

    void setPosition( const Core::Vector3& pos ) override;
    inline const Core::Vector3& getPosition() const;

    inline void setAttenuation( const Attenuation& attenuation );
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );
    inline const Attenuation& getAttenuation() const;

    std::string getShaderInclude() const override;

  private:
    Core::Vector3 m_position { 0, 0, 0 };

    Attenuation m_attenuation { 1, 0, 0 };
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/PointLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
