#ifndef RADIUMENGINE_LIGHT_HPP
#define RADIUMENGINE_LIGHT_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {
class RenderParameters;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

// FIXME (Hugo) To me this class could totally be renamed LightComponent and get a Light struct
// embedded. Thoughts are welcome !
/// A Light is an Engine Component storing a light object.
class RA_ENGINE_API Light : public Component {
  public:
    /// The type of light.
    enum LightType { DIRECTIONAL = 0, POINT, SPOT, POLYGONAL };

  public:
    RA_CORE_ALIGNED_NEW

    Light( Entity* entity, const LightType& type, const std::string& name = "light" );

    virtual ~Light();

    /// Return the color of the light.
    inline const Core::Color& getColor() const;

    /// Set the color of the light.
    inline void setColor( const Core::Color& color );

    // These function will be replaced by their use of a component -> entity
    /// Set the direction of the light, if meaningful.
    virtual void setDirection( const Core::Vector3& dir ) {}

    /// Set the position of the light, if meaningful.
    virtual void setPosition( const Core::Vector3& pos ) {}
    // ...

    /// Return the type of light.
    inline const LightType& getType() const;

    /// Return the RenderParameters for the light.
    virtual void getRenderParameters( RenderParameters& params ) const;

    /// Return the filename for the file containing the shader code to use the light.
    virtual std::string getShaderInclude() const;

    void initialize() override;

  private:
    /// The color of the light.
    Core::Color m_color;

    /// The type of the light.
    LightType m_type;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/Light.inl>

#endif // RADIUMENGINE_LIGHT_HPP
