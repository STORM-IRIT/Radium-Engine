#ifndef RADIUMENGINE_LIGHT_DATA_HPP
#define RADIUMENGINE_LIGHT_DATA_HPP

#include <memory>
#include <string>
#include <vector>

#include <Core/Containers/VectorArray.hpp>
#include <Core/File/AssetData.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>
#include <Engine/Entity/Entity.hpp>

namespace Ra {

namespace Asset {

/// The LightData class stores the data related to one of the lights of a loaded scene.
class RA_CORE_API LightData : public AssetData {

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// The type of light.
    enum LightType {
        UNKNOWN = 1 << 0,
        POINT_LIGHT = 1 << 1,
        SPOT_LIGHT = 1 << 2,
        DIRECTIONAL_LIGHT = 1 << 3,
        AREA_LIGHT = 1 << 4
    };

    /// The LightAttenuation stores the attenuation coefficients.
    /// \see Ra::Engine::PointLight::Attenuation for more info.
    struct LightAttenuation {
        /// The constant attenuation coefficient.
        Scalar constant;
        /// The linear attenuation coefficient.
        Scalar linear;
        /// The quadratic attenuation coefficient.
        Scalar quadratic;
        explicit LightAttenuation( Scalar c = 1, Scalar l = 0, Scalar q = 0 ) :
            constant( c ),
            linear( l ),
            quadratic( q ) {}
    };

    LightData( const std::string& name = "", const LightType& type = UNKNOWN );

    LightData( const LightData& data ) = default;

    ~LightData();

    /// Return the name of the Light.
    inline void setName( const std::string& name );

    /// Return the type of the Light.
    inline LightType getType() const;

    /// Set the type of the Light.
    inline void setType( const LightType& type );

    /// Return the transformation of the Light.
    inline Core::Matrix4 getFrame() const;

    /// Set the transformation of the Light.
    inline void setFrame( const Core::Matrix4& frame );

    /// Construct a directional light
    inline void setLight( Core::Color color, Core::Vector3 direction );

    /// Construct a point light
    inline void setLight( Core::Color color, Core::Vector3 position, LightAttenuation attenuation );

    /// Construct a spot light
    inline void setLight( Core::Color color, Core::Vector3 position, Core::Vector3 direction,
                          Scalar inangle, Scalar outAngle, LightAttenuation attenuation );

    /// Construct an area light
    inline void setLight( Core::Color color, LightAttenuation attenuation );

    /// Return true if the Light is a Point Light.
    inline bool isPointLight() const;

    /// Return true if the Light is a Spot Light.
    inline bool isSpotLight() const;

    /// Return true if the Light is a Directional Light.
    inline bool isDirectionalLight() const;

    /// Return true if the Light is an Area Light.
    inline bool isAreaLight() const;

    /// Print stat info to the Debug output.
    inline void displayInfo() const;

  protected:
    /// The transformation of the Light.
    Core::Matrix4 m_frame;

    /// The type of the Light.
    LightType m_type;

    // This part is public so that systems handling lights could acces to the data.
    // TODO (Mathias) : make these protected with getters ? Define independant types ?
  public:
    /// The color of the Light.
    Core::Color m_color;

    /// Actual Light data stored according to the Light type.
    union {
        struct {
            Core::Vector3 direction;
        } m_dirlight;
        struct {
            Core::Vector3 position;
            LightAttenuation attenuation;
        } m_pointlight;
        struct {
            Core::Vector3 position;
            Core::Vector3 direction;
            Scalar innerAngle;
            Scalar outerAngle;
            LightAttenuation attenuation;
        } m_spotlight;
        struct {
            // TODO (Mathias) : find how to represent arealight in a generic way
            LightAttenuation attenuation;
        } m_arealight;
    };
};

} // namespace Asset
} // namespace Ra

#include <Core/File/LightData.inl>

#endif // RADIUMENGINE_LIGHT_DATA_HPP
