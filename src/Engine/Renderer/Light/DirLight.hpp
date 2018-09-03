#ifndef RADIUMENGINE_DIRLIGHT_HPP
#define RADIUMENGINE_DIRLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

/// A DirectionalLight is a light directed along one direction, lighting from and to infinity.
class RA_ENGINE_API DirectionalLight final : public Light {
  public:
    RA_CORE_ALIGNED_NEW

    DirectionalLight( Entity* entity, const std::string& name = "dirlight" );

    ~DirectionalLight();

    void getRenderParameters( RenderParameters& params ) const override;

    void setDirection( const Core::Vector3& pos ) override;

    std::string getShaderInclude() const override;

    /// Return the lighting direction.
    inline const Core::Vector3& getDirection() const;

  private:
    /// The lighting direction.
    Core::Vector3 m_direction;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/DirLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
