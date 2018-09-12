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
#if 0
namespace Engine {
class Light;
}
#endif

namespace Asset {

class RA_CORE_API LightData : public AssetData {

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// ENUM
    enum LightType {
        UNKNOWN = 1 << 0,
        POINT_LIGHT = 1 << 1,
        SPOT_LIGHT = 1 << 2,
        DIRECTIONAL_LIGHT = 1 << 3,
        AREA_LIGHT = 1 << 4
    };

    struct LightAttenuation {
        Scalar constant;
        Scalar linear;
        Scalar quadratic;
        explicit LightAttenuation( Scalar c = 1, Scalar l = 0, Scalar q = 0 ) :
            constant( c ),
            linear( l ),
            quadratic( q ) {}
    };

    /// CONSTRUCTOR
    LightData( const std::string& name = "", const LightType& type = UNKNOWN );

    LightData( const LightData& data );

    /// DESTRUCTOR
    ~LightData();

    /// NAME
    inline void setName( const std::string& name );

    /// TYPE
    inline LightType getType() const;
    inline void setType( const LightType& type );

    /// FRAME
    inline Core::Matrix4 getFrame() const;
    inline void setFrame( const Core::Matrix4& frame );

    /// DATA
    /*
    inline std::shared_ptr<Ra::Engine::Light> getLight() const;
    inline void setLight( std::shared_ptr<Ra::Engine::Light> light );
    */
#if 0
    /// Acces to the data. The returned component must be attached to an entity after that.
    // this will add a dependence on the core to the engine. Not a good idea ...
    Ra::Engine::Light* getLight() const;
#endif

    /// construct a directional light
    inline void setLight( Core::Color color, Core::Vector3 direction );
    /// construct a point light
    inline void setLight( Core::Color color, Core::Vector3 position, LightAttenuation attenuation );
    /// construct a spot light
    inline void setLight( Core::Color color, Core::Vector3 position, Core::Vector3 direction,
                          Scalar inangle, Scalar outAngle, LightAttenuation attenuation );
    /// construct an area light
    inline void setLight( Core::Color color, Core::Vector3 cog, Core::Matrix3 spatialCov, Core::Matrix3 normalCov, LightAttenuation attenuation );

    /// QUERY
    inline bool isPointLight() const;
    inline bool isSpotLight() const;
    inline bool isDirectionalLight() const;
    inline bool isAreaLight() const;

    /// DEBUG
    inline void displayInfo() const;

  protected:
    /// VARIABLE

    Core::Matrix4 m_frame;
    LightType m_type;

    // This part is public so that systems handling lights could acces to the data.
    // TODO (Mathias) : make these protected with getters ? Define independant types ?
  public:
    Core::Color m_color;

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
            // TODO (Mathias) : this representation is usefull but might be improved
            Core::Vector3 position;
            // TODO : gives the eigenvalues/eigenvectors for spatial and normal covariance matrices.
            Core::Matrix3 spatialCovariance;
            Core::Matrix3 normalCovariance;
            LightAttenuation attenuation;
        } m_arealight;
    };
};

} // namespace Asset
} // namespace Ra

#include <Core/File/LightData.inl>

#endif // RADIUMENGINE_LIGHT_DATA_HPP
