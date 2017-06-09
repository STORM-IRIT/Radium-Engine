#include <Engine/Assets/LightData.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
    namespace Asset {


/////////////////////
/// LIGHT DATA ///
/////////////////////

/// TYPE
        inline LightData::LightType LightData::getType() const {
            return m_type;
        }

/// FRAME
        inline Core::Matrix4 LightData::getFrame() const {
            return m_frame;
        }

/// QUERY
        inline bool LightData::isPointLight() const {
            return ( m_type == POINT_LIGHT );
        }

        inline bool LightData::isSpotLight() const {
            return ( m_type == SPOT_LIGHT );
        }

        inline bool LightData::isDirectionalLight() const {
            return ( m_type == DIRECTIONAL_LIGHT );
        }

        inline bool LightData::isAreaLight() const {
            return ( m_type == AREA_LIGHT );
        }

        /// DEBUG
        inline void LightData::displayInfo() const {
            std::string type;
            switch( m_type ) {
                case UNKNOWN     : type = "UNKNOWN";       break;
                case POINT_LIGHT : type = "POINT LIGHT";   break;
                case SPOT_LIGHT   : type = "SPOT LIGHT";     break;
                case DIRECTIONAL_LIGHT    : type = "DIRECTIONAL LIGHT"; break;
                case AREA_LIGHT   : type = "AREA LIGHT";     break;
            }
            LOG( logINFO ) << "======== LIGHT INFO ========";
            LOG( logINFO ) << " Name           : " << m_name;
            LOG( logINFO ) << " Type           : " << type;
        }

/// NAME
        inline void LightData::setName( const std::string& name ) {
            m_name = name;
        }

/// TYPE
        inline void LightData::setType( const LightType& type ) {
            m_type = type;
        }

/// FRAME
        inline void LightData::setFrame( const Core::Matrix4& frame ) {
            m_frame = frame;
        }

        inline std::shared_ptr<Ra::Engine::Light> LightData::getLight() const {
            return m_light;
        }

    } // namespace Asset
} // namespace Ra
