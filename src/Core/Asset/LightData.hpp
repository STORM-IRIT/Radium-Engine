#ifndef RADIUMENGINE_LIGHT_DATA_HPP
#define RADIUMENGINE_LIGHT_DATA_HPP

#include <memory>
#include <string>
#include <vector>

#include <Core/Asset/AssetData.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/Color.hpp>

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
    enum LightType {
        UNKNOWN           = 1 << 0,
        POINT_LIGHT       = 1 << 1,
        SPOT_LIGHT        = 1 << 2,
        DIRECTIONAL_LIGHT = 1 << 3,
        AREA_LIGHT        = 1 << 4
    };

    /**
     * Define the parameters of the attenuation function.
     * The only supported attenuation function is 1/ (constant + linear*dist + quadradic*dist^2 )
     */
    struct LightAttenuation {
        Scalar constant;
        Scalar linear;
        Scalar quadratic;
        explicit LightAttenuation( Scalar c = 1, Scalar l = 0, Scalar q = 0 ) :
            constant( c ), linear( l ), quadratic( q ) {}
    };
    // TODO : allow to define other attenuation function such
    // \max\left(0,1-\frac{\left|x\right|^2}{r^2}\right)^2

    /**
     * Light constructor.
     * Must be called explicitely with a name for the light and its type.
     */
    explicit LightData( const std::string& name = "", const LightType& type = UNKNOWN );

    /**
     * Copy constructor.
     * As a Light a a union like object, no default copy constructor could be generated. This will
     * take care of copying the good members of the union.
     * @param data
     */
    LightData( const LightData& data );

    /// Destructor
    ~LightData();

    /**
     * Extension of of the AssetData interface so that we can modify an abcet afeter ts creation.
     * This has no impact on the coherence of the LightData object and could be used without
     * restriction.
     */
    inline void setName( const std::string& name );

    /**
     * Acces to the local frame of the light.
     * @return the local frame
     */
    inline const Eigen::Matrix<Scalar, 4, 4>& getFrame() const;

    /**
     * Set the local frame of the light.
     * @param frame the local frame
     */
    inline void setFrame( const Eigen::Matrix<Scalar, 4, 4>& frame );

    /**
     * Construct a directional light.
     * A directional light is only defined by its color and its lighting direction.
     * No attenuation on directional.
     *  \note The object on which this method is called is unconditionally promoted to
     * ``DIRECTIONAL_LIGHT`` light, whatever it was before the call and only the directional light
     * part of the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color,
                          const Eigen::Matrix<Scalar, 3, 1>& direction );

    /**
     * Construct a point light.
     * A point light is defined by its color, its position and its attenuation.
     *  \note The object on which this method is called is unconditionally promoted to
     * ``POINT_LIGHT`` light, whatever it was before the call and only the directional light part of
     * the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color,
                          const Eigen::Matrix<Scalar, 3, 1>& position,
                          LightAttenuation attenuation );

    /**
     * Construct a spot light.
     * A spot light is defined by its color, its position, the cone axis and light distribution
     * (constant inside inangle, quadratically deacreasing toward 0 fron inAngle to ouAngle) and its
     * attenuation. \note The object on which this method is called is unconditionally promoted to
     * ``SPOT_LIGHT`` light, whatever it was before the call and only the directional light part of
     * the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color,
                          const Eigen::Matrix<Scalar, 3, 1>& position,
                          const Eigen::Matrix<Scalar, 3, 1>& direction,
                          Scalar inAngle,
                          Scalar outAngle,
                          LightAttenuation attenuation );

    /**
     * Construct a area light.
     * An area light, (isotropic with constant emision defined by its color) is approximated by its
     * center and the covariance matrices modelling the spatial extent as well as the elliptical
     * distribution of normals. \note The object on which this method is called is unconditionally
     * promoted to ``AREA_LIGHT`` light, whatever it was before the call and only the directional
     * light part of the union is consistent after the call
     */
    inline void setLight( const Core::Utils::Color& color,
                          const Eigen::Matrix<Scalar, 3, 1>& cog,
                          const Eigen::Matrix<Scalar, 3, 3>& spatialCov,
                          const Eigen::Matrix<Scalar, 3, 3>& normalCov,
                          LightAttenuation attenuation );

    /**
     * Access to the type of the object
     */
    inline LightType getType() const;

    /**
     * Returns true if the light is a PointLight
     */
    inline bool isPointLight() const;

    /**
     * Returns true if the light is a SpotLight
     */
    inline bool isSpotLight() const;

    /**
     * Returns true if the light is a DirectionalLight
     */
    inline bool isDirectionalLight() const;

    /**
     * Returns true if the light is an AreaLight
     */
    inline bool isAreaLight() const;

    /**
     * For debugging purpose, prints out the formated content of the LightData object.
     */
    inline void displayInfo() const;

  protected:
    /// VARIABLE

    Eigen::Matrix<Scalar, 4, 4> m_frame;
    LightType m_type;

    // This part is public so that systems handling lights could access to the data.
    // TODO : make these protected with getters ? Define independant types ?
  public:
    Core::Utils::Color m_color;

    union {
        struct {
            Eigen::Matrix<Scalar, 3, 1> direction;
        } m_dirlight;
        struct {
            Eigen::Matrix<Scalar, 3, 1> position;
            LightAttenuation attenuation;
        } m_pointlight;
        struct {
            Eigen::Matrix<Scalar, 3, 1> position;
            Eigen::Matrix<Scalar, 3, 1> direction;
            Scalar innerAngle;
            Scalar outerAngle;
            LightAttenuation attenuation;
        } m_spotlight;
        struct {
            // TODO : this representation is usefull but might be improved
            Eigen::Matrix<Scalar, 3, 1> position;
            Eigen::Matrix<Scalar, 3, 3> spatialCovariance;
            Eigen::Matrix<Scalar, 3, 3> normalCovariance;
            LightAttenuation attenuation;
        } m_arealight;
    };
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/LightData.inl>

#endif // RADIUMENGINE_LIGHT_DATA_HPP
