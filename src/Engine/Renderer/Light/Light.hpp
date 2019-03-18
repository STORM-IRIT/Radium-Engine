#ifndef RADIUMENGINE_LIGHT_HPP
#define RADIUMENGINE_LIGHT_HPP

#include <Core/Utils/Color.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {
class RenderParameters;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

// Radium-V2 : this class could totally be renamed LightComponent and get a Light struct embedded.
// Thoughts are welcome !
/**
 * A Light is a RadiumEngine Component storing a light object.
 */
class RA_ENGINE_API Light : public Component {
  public:
    /**
     * Supported Light types.
     * \note POLYGONAL is not really supported.
     * \todo Replace this enum by something extensible as we plan to add
     *       new Light types through plugins.
     */
    enum LightType { DIRECTIONAL = 0, POINT, SPOT, POLYGONAL };

    /**
     * Define the attenuation of the light source
     * \todo For the moment, the attenuation is non physically coherent.
     *       Make attenuation be quadratic by default.
     */
    struct Attenuation {
        Scalar constant{1};
        Scalar linear{0};
        Scalar quadratic{0};
        Attenuation() = default;
    };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Create a new Light with the given name.
     * The new Light is a Component attached to the given Entity.
     */
    Light( Entity* entity, const LightType& type, const std::string& name = "light" );

    ~Light() override = default;

    /**
     * Return the color of the Light.
     */
    inline const Core::Utils::Color& getColor() const;

    /**
     * Set the color of the Light.
     */
    inline void setColor( const Core::Utils::Color& color );

    /**
     * Set the lighting direction.
     * \todo Put this only on directional and spot light sources.
     */
    virtual void setDirection( const Eigen::Matrix<Scalar, 3, 1>& /*dir*/ ) {}

    /**
     * Set the position of the light source.
     * \todo Put this only on point and spot light sources.
     */
    virtual void setPosition( const Eigen::Matrix<Scalar, 3, 1>& /*pos*/ ) {}

    /**
     * Return the type (an enum Light::LightType member) of the Light source.
     */
    inline const LightType& getType() const;

    /**
     * Extract the set of parameters that must be given to a shader for
     * rendering and lighting with this Light.
     */
    virtual void getRenderParameters( RenderParameters& params ) const;

    /**
     * Abstract method that define the glsl code that manage this Light type.
     * \note For the moment, this is not used (except by experimental plugins),
     *       but will part of the shader composition process of radium v2.
     */
    virtual std::string getShaderInclude() const;

    /**
     * Nothing to initialize here.
     */
    void initialize() override;

  private:
    /// The color of the Light.
    Core::Utils::Color m_color = Core::Utils::Color::White();

    /// The type of the Light.
    LightType m_type{LightType::DIRECTIONAL};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/Light.inl>

#endif // RADIUMENGINE_LIGHT_HPP
