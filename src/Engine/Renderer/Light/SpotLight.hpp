#ifndef RADIUMENGINE_SPOTLIGHT_HPP
#define RADIUMENGINE_SPOTLIGHT_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/Math.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra {
namespace Engine {

class RA_ENGINE_API SpotLight final : public Light {
  public:
    struct Attenuation {
        Scalar constant;
        Scalar linear;
        Scalar quadratic;

        Attenuation() : constant( 1.0 ), linear(), quadratic() {}
    };

  public:
    RA_CORE_ALIGNED_NEW

    SpotLight( Entity* entity, const std::string& name = "spotlight" );
    ~SpotLight();

    void getRenderParameters( RenderParameters& params ) const override;

    void setPosition( const Core::Math::Vector3& position ) override;
    inline const Core::Math::Vector3& getPosition() const;

    void setDirection( const Core::Math::Vector3& direction ) override;
    inline const Core::Math::Vector3& getDirection() const;

    inline void setInnerAngleInRadians( Scalar angle );
    inline void setOuterAngleInRadians( Scalar angle );
    inline void setInnerAngleInDegrees( Scalar angle );
    inline void setOuterAngleInDegrees( Scalar angle );

    inline Scalar getInnerAngle() const;
    inline Scalar getOuterAngle() const;

    inline void setAttenuation( const Attenuation& attenuation );
    inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );
    inline const Attenuation& getAttenuation() const;

    std::string getShaderInclude() const;

  private:
    Core::Math::Vector3 m_position;
    Core::Math::Vector3 m_direction;

    Scalar m_innerAngle;
    Scalar m_outerAngle;

    Attenuation m_attenuation;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/SpotLight.inl>

#endif // RADIUMENGINE_SPOTLIGHT_HPP
