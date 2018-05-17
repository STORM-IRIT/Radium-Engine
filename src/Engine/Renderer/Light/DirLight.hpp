#ifndef RADIUMENGINE_DIRLIGHT_HPP
#define RADIUMENGINE_DIRLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

class RA_ENGINE_API DirectionalLight final : public Light {
  public:
    RA_CORE_ALIGNED_NEW

    DirectionalLight( Entity* entity, const std::string& name = "dirlight" );
    ~DirectionalLight();

    void getRenderParameters( RenderParameters& params ) const override;

    void setDirection( const Core::Math::Vector3& pos ) override;
    inline const Core::Math::Vector3& getDirection() const;

    std::string getShaderInclude() const;

  private:
    Core::Math::Vector3 m_direction;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/DirLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
