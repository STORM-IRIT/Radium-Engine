#ifndef RADIUMENGINE_POINTLIGHT_HPP
#define RADIUMENGINE_POINTLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

/**
 * A PointLight is a light spreading in all directions around an emission point.
 */
class RA_ENGINE_API PointLight final : public Light {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    explicit PointLight( Entity* entity, const std::string& name = "pointlight" );
    ~PointLight() override = default;

    void getRenderParameters( RenderParameters& params ) const override;

    std::string getShaderInclude() const override;

    void setPosition( const Eigen::Matrix<Scalar, 3, 1>& pos ) override;

    /**
     * Return the emission point.
     */
    inline const Eigen::Matrix<Scalar, 3, 1>& getPosition() const;

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

  private:
    /// The emission point.
    Eigen::Matrix<Scalar, 3, 1> m_position{0, 0, 0};

    /// The Attenuation coefficients.
    Attenuation m_attenuation{1, 0, 0};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/PointLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
