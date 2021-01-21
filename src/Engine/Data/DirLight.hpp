#pragma once

#include <Engine/Data/Light.hpp>
#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {

namespace Scene {
class Entity;
}

namespace Data {
/** Directional light for rendering
 *
 */
class RA_ENGINE_API DirectionalLight final : public Light
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    explicit DirectionalLight( Scene::Entity* entity, const std::string& name = "dirlight" );
    ~DirectionalLight() override = default;

    void getRenderParameters( Renderer::RenderParameters& params ) const override;

    void setDirection( const Eigen::Matrix<Scalar, 3, 1>& dir ) override;
    inline const Eigen::Matrix<Scalar, 3, 1>& getDirection() const;

    std::string getShaderInclude() const override;

  private:
    Eigen::Matrix<Scalar, 3, 1> m_direction {0, -1, 0};
};
} // namespace Data
} // namespace Engine
} // namespace Ra

#include <Engine/Data/DirLight.inl>
