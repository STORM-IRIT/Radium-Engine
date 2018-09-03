#ifndef RADIUMENGINE_SPOTLIGHT_HPP
#define RADIUMENGINE_SPOTLIGHT_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/Math.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

/// A SpotLight is a light spreading in a given direction around an emission
/// point with a fixed aperture angle, leading to a cone of light.
class RA_ENGINE_API SpotLight final : public Light {
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

    SpotLight( Entity* entity, const std::string& name = "spotlight" );

    ~SpotLight();

    void getRenderParameters( RenderParameters& params ) const override;

    void setPosition( const Core::Vector3& position ) override;

    void setDirection( const Core::Vector3& direction ) override;

    std::string getShaderInclude() const override;

    /// Return the emmision point.
    inline const Core::Vector3& getPosition() const;

    /// Return the lighting direction.
    inline const Core::Vector3& getDirection() const;

    /// Set the emission cone radius (twice the angle from the direction to the boundary).
    inline void setInnerAngleInRadians( Scalar angle );

    /// Set the emission cone radius (twice the angle from the direction to the boundary).
    inline void setInnerAngleInDegrees( Scalar angle );

    // FIXME: this one if never used.
    inline void setOuterAngleInRadians( Scalar angle );
    inline void setOuterAngleInDegrees( Scalar angle );
    inline Scalar getOuterAngle() const;

    /// Return the emission cone radius.
    inline Scalar getInnerAngle() const;

    /// Set the Attenuation for the light.
    inline void setAttenuation( const Attenuation& attenuation );

    /// Set the Attenuation for the light.
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );

    /// Return the Attenuation for the light.
    inline const Attenuation& getAttenuation() const;

  private:
    /// The emission point.
    Core::Vector3 m_position;

    /// The lighting direction.
    Core::Vector3 m_direction;

    /// The emissio cone radius.
    Scalar m_innerAngle;

    // FIXME: this one is never used! What does it correspond to?
    Scalar m_outerAngle;

    /// The attenuation coefficients.
    Attenuation m_attenuation;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/SpotLight.inl>

#endif // RADIUMENGINE_SPOTLIGHT_HPP
