#ifndef RADIUMENGINE_SPOTLIGHT_HPP
#define RADIUMENGINE_SPOTLIGHT_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/Math.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

/**
 * A SpotLight is a light spreading in a given direction around an emission
 * point with a fixed aperture angle, leading to a cone of light.
 */
class RA_ENGINE_API SpotLight final : public Light {

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    explicit SpotLight( Entity* entity, const std::string& name = "spotlight" );
    ~SpotLight() override = default;

    void getRenderParameters( RenderParameters& params ) const override;

    void setPosition( const Eigen::Matrix<Scalar, 3, 1>& position ) override;

    void setDirection( const Eigen::Matrix<Scalar, 3, 1>& direction ) override;

    /**
     * Return the emmision point.
     */
    inline const Eigen::Matrix<Scalar, 3, 1>& getPosition() const;

    /**
     * Return the lighting direction.
     */
    inline const Eigen::Matrix<Scalar, 3, 1>& getDirection() const;

    /**
     * Set the emission cone inner radius, within which there is no light attenuation.
     * It corresponds to twice the angle from the direction to the boundary.
     */
    inline void setInnerAngleInRadians( Scalar angle );

    /**
     * Set the emission cone inner radius, within which there is no light attenuation.
     * It corresponds to twice the angle from the direction to the boundary.
     */
    inline void setInnerAngleInDegrees( Scalar angle );

    /**
     * Return the emission cone inner radius, within which there is no light attenuation.
     * It corresponds to twice the angle from the direction to the boundary.
     */
    inline Scalar getInnerAngle() const;

    /**
     * Return the emission cone outer radius, within which light attenuation apply.
     * It corresponds to the zone directly outside the inner radius.
     * \bug unused.
     */
    inline void setOuterAngleInRadians( Scalar angle );

    /**
     * Return the emission cone outer radius, within which light attenuation apply.
     * It corresponds to the zone directly outside the inner radius.
     * \bug unused.
     */
    inline void setOuterAngleInDegrees( Scalar angle );

    /**
     * Set the emission cone outer radius, within which light attenuation apply.
     * It corresponds to the zone directly outside the inner radius.
     * \bug unused.
     */
    inline Scalar getOuterAngle() const;

    /**
     * Set the Attenuation coefficients.
     */
    inline void setAttenuation( const Attenuation& attenuation );

    /**
     * Set the Attenuation coefficients.
     */
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );

    /**
     * Return the Attenuation coefficients.
     */
    inline const Attenuation& getAttenuation() const;

    std::string getShaderInclude() const override;

  private:
    /// The emission point.
    Eigen::Matrix<Scalar, 3, 1> m_position{0, 0, 0};

    /// The lighting direction.
    Eigen::Matrix<Scalar, 3, 1> m_direction{0, -1, 0};

    /// The emission cone inner radius.
    Scalar m_innerAngle{Core::Math::PiDiv4};

    /// The emission cone outer radius.
    Scalar m_outerAngle{Core::Math::PiDiv2};

    /// The Attenuation coefficients.
    Attenuation m_attenuation{1, 0, 0};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/SpotLight.inl>

#endif // RADIUMENGINE_SPOTLIGHT_HPP
