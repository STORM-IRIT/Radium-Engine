#ifndef RADIUMENGINE_POINTLIGHT_HPP
#define RADIUMENGINE_POINTLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

class RA_ENGINE_API PointLight final : public Light {
  public:
    struct Attenuation {
        Scalar constant;
        Scalar linear;
        Scalar quadratic;

        Attenuation() : constant( 1.0 ), linear(), quadratic() {}
    };

  public:
    RA_CORE_ALIGNED_NEW

    PointLight( const std::string& name = "pointlight" );
    ~PointLight();

    void getRenderParameters( RenderParameters& params ) override;

    void setPosition( const Core::Vector3& pos ) override;
    inline const Core::Vector3& getPosition() const;

    inline void setAttenuation( const Attenuation& attenuation );
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );
    inline const Attenuation& getAttenuation() const;

    std::string getShaderInclude() const;

  private:
    Core::Vector3 m_position;

    Attenuation m_attenuation;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/PointLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
