#ifndef RADIUMENGINE_LIGHT_DATA_HPP
#define RADIUMENGINE_LIGHT_DATA_HPP

#include <string>
#include <vector>
#include <memory>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>

#include <Engine/Assets/AssetData.hpp>

namespace Ra {
    namespace Engine
    {
        class Light;
    }

    namespace Asset {

class LightData : public AssetData {

    /// FRIEND
    friend class AssimpLightDataLoader;

public:

    RA_CORE_ALIGNED_NEW

    /// ENUM
    enum LightType {
        UNKNOWN             = 1 << 0,
        POINT_LIGHT         = 1 << 1,
        SPOT_LIGHT          = 1 << 2,
        DIRECTIONAL_LIGHT   = 1 << 3,
        AREA_LIGHT          = 1 << 4
    };

    /// CONSTRUCTOR
    LightData( const std::string&  name = "",
                  const LightType& type = UNKNOWN );

    LightData( const LightData& data ) = default;

    /// DESTRUCTOR
    ~LightData();

    /// TYPE
    inline LightType getType() const;

    /// FRAME
    inline Core::Matrix4 getFrame() const;

    /// DATA
    inline std::shared_ptr<Ra::Engine::Light> getLight() const;

    /// QUERY
    inline bool isPointLight() const;
    inline bool isSpotLight() const;
    inline bool isDirectionalLight() const;
    inline bool isAreaLight() const;


    /// DEBUG
    inline void displayInfo() const;

protected:
    /// NAME
    inline void setName( const std::string& name );

    /// TYPE
    inline void setType( const LightType& type );

    /// FRAME
    inline void setFrame( const Core::Matrix4& frame );


protected:
    /// VARIABLE

    Core::Matrix4 m_frame;
    LightType    m_type;

    std::shared_ptr<Ra::Engine::Light> m_light;

};

} // namespace Asset
} // namespace Ra

#include <Engine/Assets/LightData.inl>

#endif // RADIUMENGINE_LIGHT_DATA_HPP
