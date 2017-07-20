#ifndef RADIUMENGINE_LIGHT_DATA_HPP
#define RADIUMENGINE_LIGHT_DATA_HPP

#include <string>
#include <vector>
#include <memory>

#include <Core/RaCore.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/File/AssetData.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
    namespace Engine
    {
        class Light;
    }

    namespace Asset {

class RA_CORE_API LightData : public AssetData {

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

    /// NAME
    inline void setName( const std::string& name );

    /// TYPE
    inline LightType getType() const;
    inline void setType( const LightType& type );

    /// FRAME
    inline Core::Matrix4 getFrame() const;
    inline void setFrame( const Core::Matrix4& frame );

    /// DATA
    inline std::shared_ptr<Ra::Engine::Light> getLight() const;
    inline void setLight( std::shared_ptr<Ra::Engine::Light> light);

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
    LightType    m_type;

    std::shared_ptr<Ra::Engine::Light> m_light;

};

} // namespace Asset
} // namespace Ra

#include <Core/File/LightData.inl>

#endif // RADIUMENGINE_LIGHT_DATA_HPP
