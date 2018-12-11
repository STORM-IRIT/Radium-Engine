#ifndef RADIUMENGINE_DIRLIGHT_HPP
#define RADIUMENGINE_DIRLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

  /** Directional light for rendering
   *
   */
class RA_ENGINE_API DirectionalLight final : public Light {
  public:
    RA_CORE_ALIGNED_NEW

    explicit DirectionalLight( Entity* entity, const std::string& name = "dirlight" );
    ~DirectionalLight() override = default;

    void getRenderParameters( RenderParameters& params ) const override;

    void setDirection( const Core::Vector3& dir ) override;
    inline const Core::Vector3& getDirection() const;

    std::string getShaderInclude() const override;

  private:
    Core::Vector3 m_direction { 0, -1, 0 };
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/DirLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
