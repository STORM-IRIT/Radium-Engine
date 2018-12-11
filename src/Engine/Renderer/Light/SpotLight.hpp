#ifndef RADIUMENGINE_SPOTLIGHT_HPP
#define RADIUMENGINE_SPOTLIGHT_HPP

#include <Engine/RaEngine.hpp>

#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

  /** Spot light for rendering.
   *
   */
  class RA_ENGINE_API SpotLight final : public Light {

  public:
    RA_CORE_ALIGNED_NEW

    explicit SpotLight( Entity* entity, const std::string& name = "spotlight" );
    ~SpotLight() override = default;

    void getRenderParameters( RenderParameters& params ) const override;

    void setPosition( const Core::Vector3& position ) override;
    inline const Core::Vector3& getPosition() const;

    void setDirection( const Core::Vector3& direction ) override;
    inline const Core::Vector3& getDirection() const;

    inline void setInnerAngleInRadians( Scalar angle );
    inline void setOuterAngleInRadians( Scalar angle );
    inline void setInnerAngleInDegrees( Scalar angle );
    inline void setOuterAngleInDegrees( Scalar angle );

    inline Scalar getInnerAngle() const;
    inline Scalar getOuterAngle() const;

    inline void setAttenuation( const Attenuation& attenuation );
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );
    inline const Attenuation& getAttenuation() const;

    std::string getShaderInclude() const override;

  private:
    Core::Vector3 m_position { 0, 0, 0 };
    Core::Vector3 m_direction { 0, -1, 0 };

    Scalar m_innerAngle { Core::Math::PiDiv4 };
    Scalar m_outerAngle { Core::Math::PiDiv2 };

    Attenuation m_attenuation { 1, 0, 0 };
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/SpotLight.inl>

#endif // RADIUMENGINE_SPOTLIGHT_HPP
