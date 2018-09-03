#ifndef RADIUMENGINE_POINTLIGHT_HPP
#define RADIUMENGINE_POINTLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

/// A PointLight is a light spreading in all directions around an emission point.
class RA_ENGINE_API PointLight final : public Light {
  public:
    /// The Attenuation of a light is computed as:
    ///    \f$ att = a*d^2 + b*d + c \f$
    /// where \f$ d \f$ is the norm of the lighting vector in the BlinnPhong lighting model.
    struct Attenuation {
        /// The constant attenuation coefficient.
        Scalar constant;
        /// The linear attenuation coefficient.
        Scalar linear;
        /// The quadratic attenuation coefficient.
        Scalar quadratic;

        Attenuation() : constant( 1.0 ), linear(), quadratic() {}
    };

  public:
    RA_CORE_ALIGNED_NEW

    PointLight( Entity* entity, const std::string& name = "pointlight" );
    ~PointLight();

    void getRenderParameters( RenderParameters& params ) const override;

    void setPosition( const Core::Vector3& pos ) override;

    std::string getShaderInclude() const override;

    /// Return the emission point.
    inline const Core::Vector3& getPosition() const;

    /// Set the Attenuation coefficients.
    inline void setAttenuation( const Attenuation& attenuation );

    /// Set the Attenuation coefficients.
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );

    /// Return the Attenuation coefficients.
    inline const Attenuation& getAttenuation() const;

  private:
    /// The emission point.
    Core::Vector3 m_position;

    /// The Attenuation coefficients.
    Attenuation m_attenuation;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/PointLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
