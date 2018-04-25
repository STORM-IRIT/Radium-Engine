#ifndef RADIUMENGINE_LIGHT_HPP
#define RADIUMENGINE_LIGHT_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {
class RenderParameters;
}
} // namespace Ra

namespace Ra {
namespace Engine {

// FIXME (Hugo) To me this class could totally be renamed LightComponent and get a Light struct
// embedded. Thoughts are welcome !
class RA_ENGINE_API Light : public Component {
  public:
    enum LightType { DIRECTIONAL = 0, POINT, SPOT, POLYGONAL };

  public:
    RA_CORE_ALIGNED_NEW

    Light( Entity* entity, const LightType& type, const std::string& name = "light" );
    virtual ~Light();

    inline const Core::Math::Color& getColor() const;
    inline void setColor( const Core::Math::Color& color );

    // These function will be replaced by their use of a component -> entity
    virtual void setDirection( const Core::Math::Vector3& dir ) {}
    virtual void setPosition( const Core::Math::Vector3& pos ) {}
    // ...

    inline const LightType& getType() const;

    virtual void getRenderParameters( RenderParameters& params ) const;

    virtual std::string getShaderInclude() const;

    void initialize() override;

  private:
    Core::Math::Color m_color;

    LightType m_type;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/Light.inl>

#endif // RADIUMENGINE_LIGHT_HPP
