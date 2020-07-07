#ifndef RADIUMENGINE_DIRLIGHT_HPP
#define RADIUMENGINE_DIRLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

/** Directional light for rendering
 *
 */
class RA_ENGINE_API DirectionalLight final : public Light
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    explicit DirectionalLight( Entity* entity, const std::string& name = "dirlight" );
    ~DirectionalLight() override = default;

    void getRenderParameters( RenderParameters& params ) const override;

    void setDirection( const Eigen::Matrix<Scalar, 3, 1>& dir ) override;
    inline const Eigen::Matrix<Scalar, 3, 1>& getDirection() const;

    std::string getShaderInclude() const override;

  private:
    Eigen::Matrix<Scalar, 3, 1> m_direction {0, -1, 0};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/DirLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
