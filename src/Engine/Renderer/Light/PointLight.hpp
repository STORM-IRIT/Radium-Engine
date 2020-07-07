#ifndef RADIUMENGINE_POINTLIGHT_HPP
#define RADIUMENGINE_POINTLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

/** Point light for rendering
 *
 */
class RA_ENGINE_API PointLight final : public Light
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    explicit PointLight( Entity* entity, const std::string& name = "pointlight" );
    ~PointLight() override = default;

    void getRenderParameters( RenderParameters& params ) const override;

    void setPosition( const Eigen::Matrix<Scalar, 3, 1>& pos ) override;
    inline const Eigen::Matrix<Scalar, 3, 1>& getPosition() const;

    inline void setAttenuation( const Attenuation& attenuation );
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );
    inline const Attenuation& getAttenuation() const;

    std::string getShaderInclude() const override;

  private:
    Eigen::Matrix<Scalar, 3, 1> m_position {0, 0, 0};

    Attenuation m_attenuation {1, 0, 0};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/PointLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
