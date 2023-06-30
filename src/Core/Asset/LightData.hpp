#pragma once
#include <Core/RaCore.hpp>

#include <Core/Asset/AssetData.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>

#include <memory>
#include <string>
#include <vector>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * Asset defining how lights must be passed to the Radium systems from a loader.
 *
 * Supported light type are :
 *    Point light : a local, punctual, isotropic spherical light.
 *    Spot light : a local, punctual, isotropic conical light.
 *    Directional light : an infinite, isotropic light.
 *    Area light : a local isotropic area light.
 */
class RA_CORE_API LightData : public AssetData
{

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /** Supported light type
     *
     */
    enum LightType : unsigned int {
        UNKNOWN           = 1u << 0u,
        POINT_LIGHT       = 1u << 1u,
        SPOT_LIGHT        = 1u << 2u,
        DIRECTIONAL_LIGHT = 1u << 3u,
        AREA_LIGHT        = 1u << 4u
    };

    /**
     * Define the parameters of the attenuation function.
     * The only supported attenuation function is 1/ (constant + linear*dist + quadratic*dist^2 )
     */
    struct LightAttenuation {
        Scalar constant { 1_ra };
        Scalar linear { 0_ra };
        Scalar quadratic { 0_ra };
        explicit LightAttenuation( Scalar c = 1_ra, Scalar l = 0_ra, Scalar q = 0_ra ) :
            constant( c ), linear( l ), quadratic( q ) {}
    };
    // TODO : allow to define other attenuation function such
    // \max\left(0,1-\frac{\left|x\right|^2}{r^2}\right)^2

    /**
     * Light constructor.
     * Must be called explicitly with a name for the light and its type.
     */
    explicit LightData( const std::string& name = "", const LightType& type = UNKNOWN ) :
        AssetData( name ), m_type( type ) {}

    /**
     * Copy constructor.
     * As a Light a a union like object, no default copy constructor could be generated. This will
     * take care of copying the good members of the union.
     * @param data
     */
    LightData( const LightData& data );

    /**
     * Access to the local frame of the light.
     * @return the local frame
     */
    inline const Matrix4& getFrame() const { return m_frame; }

    /**
     * Set the local frame of the light.
     * @param frame the local frame
     */
    inline void setFrame( const Matrix4& frame ) { m_frame = frame; }

    /**
     * Construct a directional light.
     * A directional light is only defined by its color and its lighting direction.
     * No attenuation on directional.
     *  \note The object on which this method is called is unconditionally promoted to
     * ``DIRECTIONAL_LIGHT`` light, whatever it was before the call and only the directional light
     * part of the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color, const Vector3& direction );

    /**
     * Construct a point light.
     * A point light is defined by its color, its position and its attenuation.
     *  \note The object on which this method is called is unconditionally promoted to
     * ``POINT_LIGHT`` light, whatever it was before the call and only the directional light part of
     * the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color,
                          const Vector3& position,
                          LightAttenuation attenuation );

    /**
     * Construct a spot light.
     * A spot light is defined by its color, its position, the cone axis and light distribution
     * (constant inside inAngle, quadratically decreasing toward 0 from inAngle to ouAngle) and its
     * attenuation. \note The object on which this method is called is unconditionally promoted to
     * ``SPOT_LIGHT`` light, whatever it was before the call and only the directional light part of
     * the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color,
                          const Vector3& position,
                          const Vector3& direction,
                          Scalar inAngle,
                          Scalar outAngle,
                          LightAttenuation attenuation );

    /**
     * Construct a area light.
     * An area light, (isotropic with constant emission defined by its color) is approximated by its
     * center and the covariance matrices modelling the spatial extent as well as the elliptical
     * distribution of normals. \note The object on which this method is called is unconditionally
     * promoted to ``AREA_LIGHT`` light, whatever it was before the call and only the directional
     * light part of the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color,
                          const Vector3& cog,
                          const Matrix3& spatialCov,
                          const Matrix3& normalCov,
                          LightAttenuation attenuation );

    /**
     * Access to the type of the object
     */
    inline LightType getType() const { return m_type; }

    /**
     * Returns true if the light is a PointLight
     */
    inline bool isPointLight() const { return m_type == POINT_LIGHT; }

    /**
     * Returns true if the light is a SpotLight
     */
    inline bool isSpotLight() const { return m_type == SPOT_LIGHT; }

    /**
     * Returns true if the light is a DirectionalLight
     */
    inline bool isDirectionalLight() const { return m_type == DIRECTIONAL_LIGHT; }

    /**
     * Returns true if the light is an AreaLight
     */
    inline bool isAreaLight() const { return m_type == AREA_LIGHT; }

    /**
     * For debugging purpose, prints out the formatted content of the LightData object.
     */
    void displayInfo() const;

  protected:
    /// VARIABLE

    Matrix4 m_frame { Matrix4::Identity() };
    LightType m_type { UNKNOWN };

    // This part is public so that systems handling lights could access to the data.
    // TODO : make these protected with getters ? Define independent types ?
  public:
    Core::Utils::Color m_color { Utils::Color::White() };

    struct DirLight {
        Vector3 direction;
    };
    struct PointLight {
        Vector3 position;
        LightAttenuation attenuation;
    };
    struct SpotLight {
        Vector3 position;
        Vector3 direction;
        Scalar innerAngle;
        Scalar outerAngle;
        LightAttenuation attenuation;
    };
    struct AreaLight {
        // TODO : this representation is useful but might be improved
        Vector3 position;
        Matrix3 spatialCovariance;
        Matrix3 normalCovariance;
        LightAttenuation attenuation;
    };

    union {
        DirLight m_dirlight;
        PointLight m_pointlight;
        SpotLight m_spotlight;
        AreaLight m_arealight;
    };
};

/////////////////////
///  LIGHT DATA   ///
/////////////////////

/// construct a directional light
inline void LightData::setLight( const Core::Utils::Color& color, const Vector3& direction ) {
    m_type               = DIRECTIONAL_LIGHT;
    m_color              = color;
    m_dirlight.direction = direction;
}

/// construct a point light
inline void LightData::setLight( const Core::Utils::Color& color,
                                 const Vector3& position,
                                 LightAttenuation attenuation ) {
    m_type                   = POINT_LIGHT;
    m_color                  = color;
    m_pointlight.position    = position;
    m_pointlight.attenuation = attenuation;
}

/// construct a spot light
inline void LightData::setLight( const Core::Utils::Color& color,
                                 const Vector3& position,
                                 const Vector3& direction,
                                 Scalar inAngle,
                                 Scalar outAngle,
                                 LightAttenuation attenuation ) {
    m_type                  = SPOT_LIGHT;
    m_color                 = color;
    m_spotlight.position    = position;
    m_spotlight.direction   = direction;
    m_spotlight.innerAngle  = inAngle;
    m_spotlight.outerAngle  = outAngle;
    m_spotlight.attenuation = attenuation;
}

/// construct an area light
inline void LightData::setLight( const Core::Utils::Color& color,
                                 const Vector3& cog,
                                 const Matrix3& spatialCov,
                                 const Matrix3& normalCov,
                                 LightAttenuation attenuation ) {
    m_type                        = AREA_LIGHT;
    m_color                       = color;
    m_arealight.position          = cog;
    m_arealight.spatialCovariance = spatialCov;
    m_arealight.normalCovariance  = normalCov;
    m_arealight.attenuation       = attenuation;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
