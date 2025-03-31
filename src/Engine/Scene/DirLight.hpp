#pragma once

#include <Core/CoreMacros.hpp>
#include <Eigen/Core>
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/RaEngine.hpp>
#include <Engine/Scene/Light.hpp>
#include <map>
#include <string>

namespace Ra {
namespace Engine {
namespace Scene {

class Entity;

/** Directional light for rendering
 *
 */
class RA_ENGINE_API DirectionalLight final : public Ra::Engine::Scene::Light
{
  public:
    explicit DirectionalLight( Entity* entity, const std::string& name = "dirlight" );
    ~DirectionalLight() override = default;

    void setDirection( const Eigen::Matrix<Scalar, 3, 1>& dir ) override;
    inline const Eigen::Matrix<Scalar, 3, 1>& getDirection() const;

    std::string getShaderInclude() const override;

  private:
    Eigen::Matrix<Scalar, 3, 1> m_direction { 0, -1, 0 };
};

// ---------------------------------------------------------------------------------------------
// ---- inline methods implementation

inline void DirectionalLight::setDirection( const Eigen::Matrix<Scalar, 3, 1>& dir ) {
    m_direction = dir.normalized();
    getRenderParameters().setVariable( "light.directional.direction", m_direction );
}

inline const Eigen::Matrix<Scalar, 3, 1>& DirectionalLight::getDirection() const {
    return m_direction;
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
