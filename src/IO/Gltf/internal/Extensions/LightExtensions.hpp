#pragma once
#include <IO/Gltf/internal/fx/gltf.h>

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual
 *
 */

/// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/schema/light.spot.schema.json
struct gltf_lightSpot {
    /// Angle in radians from centre of spotlight where falloff begins.
    ///  min  : 0
    ///  max (exclusive) : pi/2
    float innerConeAngle { 0.0 };
    /// Angle in radians from centre of spotlight where falloff ends.
    /// min  : 0
    /// max (exclusive) : pi/2
    float outerConeAngle { 0.7853981633974483 };
    nlohmann::json extensionsAndExtras {};

    [[nodiscard]] bool empty() const {
        return innerConeAngle == 0.f && outerConeAngle == 0.7853981633974483f;
    }
};

inline void from_json( nlohmann::json const& json, gltf_lightSpot& lightSpot ) {
    fx::gltf::detail::ReadOptionalField( "innerConeAngle", json, lightSpot.innerConeAngle );
    fx::gltf::detail::ReadOptionalField( "outerConeAngle", json, lightSpot.outerConeAngle );

    fx::gltf::detail::ReadExtensionsAndExtras( json, lightSpot.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json, gltf_lightSpot const& lightSpot ) {
    fx::gltf::detail::WriteField( { "innerConeAngle" }, json, lightSpot.innerConeAngle, 0.0f );
    fx::gltf::detail::WriteField(
        { "outerConeAngle" }, json, lightSpot.outerConeAngle, 0.7853981633974483f );
    fx::gltf::detail::WriteExtensions( json, lightSpot.extensionsAndExtras );
}

/// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/schema/light.schema.json
struct gltf_lightPunctual {
    enum class Type {
        /// Directional lights act as though they are infinitely far away and emit light in the
        /// direction of the local -z axis. This light type inherits the orientation of the node
        /// that it belongs to; position and scale are ignored except for their effect on the
        /// inherited node orientation. Because it is at an infinite distance, the light is not
        /// attenuated. Its intensity is defined in lumens per metre squared, or lux (lm/m^2).
        directional,
        /// Point lights emit light in all directions from their position in space; rotation and
        /// scale are ignored except for their effect on the inherited node position. `
        /// The brightness of the light attenuates in a physically correct manner as distance
        /// increases from the light's position (i.e. brightness goes like the inverse square of
        /// the distance). Point light intensity is defined in candela, which is lumens per square
        /// radian (lm/sr).
        point,
        /// Spot lights emit light in a cone in the direction of the local -z axis. The angle
        /// and falloff of the cone is defined using two numbers, the innerConeAngle and
        /// outerConeAngle. As with point lights, the brightness also attenuates in a physically
        /// correct manner as distance increases from the light's position (i.e. brightness goes
        /// like the inverse square of the distance). Spot light intensity refers to the brightness
        /// inside the innerConeAngle (and at the location of the light) and is defined in candela,
        /// which is lumens per square radian (lm/sr). Engines that don't support two angles
        /// for spotlights should use outerConeAngle as the spotlight angle (leaving innerConeAngle
        /// to implicitly be 0).
        spot,
        None
    };
    /// The type of the light source
    Type type { Type::None };
    /// Color of the light source.
    std::array<float, 3> color = { fx::gltf::defaults::IdentityVec3 };
    /// Intensity of the light source. `point` and `spot` lights use luminous intensity in
    /// candela (lm/sr) while `directional` lights use illuminance in lux (lm/m^2).
    float intensity { 1.0 };
    /// spot properties
    gltf_lightSpot spot;
    /// A distance cutoff at which the light's intensity may be considered to have reached zero.
    /// attenuation = max( min( 1.0 - ( current_distance / range )^4, 1 ), 0 ) / current_distance^2
    float range { std::numeric_limits<float>::max() };
    /// Name of the light source
    std::string name {};
    /// extensions and extra
    nlohmann::json extensionsAndExtras {};
};

inline void from_json( nlohmann::json const& json, gltf_lightPunctual::Type& lightType ) {
    std::string type = json.get<std::string>();
    if ( type == "directional" ) { lightType = gltf_lightPunctual::Type::directional; }
    else if ( type == "point" ) { lightType = gltf_lightPunctual::Type::point; }
    else if ( type == "spot" ) { lightType = gltf_lightPunctual::Type::spot; }
    else { throw fx::gltf::invalid_gltf_document( "Unknown lights_punctual.type value", type ); }
}

inline void from_json( nlohmann::json const& json, gltf_lightPunctual& lightPunctual ) {
    fx::gltf::detail::ReadRequiredField( "type", json, lightPunctual.type );
    fx::gltf::detail::ReadOptionalField( "name", json, lightPunctual.name );
    fx::gltf::detail::ReadOptionalField( "color", json, lightPunctual.color );
    fx::gltf::detail::ReadOptionalField( "intensity", json, lightPunctual.intensity );
    fx::gltf::detail::ReadOptionalField( "range", json, lightPunctual.range );
    if ( lightPunctual.type == gltf_lightPunctual::Type::spot ) {
        fx::gltf::detail::ReadOptionalField( "spot", json, lightPunctual.spot );
    }
    fx::gltf::detail::ReadExtensionsAndExtras( json, lightPunctual.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json, gltf_lightPunctual::Type& lightType ) {
    switch ( lightType ) {
    case gltf_lightPunctual::Type::directional:
        json = "directional";
        break;
    case gltf_lightPunctual::Type::point:
        json = "point";
        break;
    case gltf_lightPunctual::Type::spot:
        json = "spot";
        break;
    default:
        throw fx::gltf::invalid_gltf_document( "Unknown lights_punctual.type value" );
    }
}

inline void to_json( nlohmann::json& json, gltf_lightPunctual const& lightPunctual ) {
    fx::gltf::detail::WriteField( "name", json, lightPunctual.name );
    fx::gltf::detail::WriteField(
        "type", json, lightPunctual.type, gltf_lightPunctual::Type::None );
    fx::gltf::detail::WriteField( "color", json, lightPunctual.color );
    fx::gltf::detail::WriteField( "intensity", json, lightPunctual.intensity, 1.0f );
    fx::gltf::detail::WriteField(
        "range", json, lightPunctual.range, std::numeric_limits<float>::max() );
    if ( lightPunctual.type == gltf_lightPunctual::Type::spot ) {
        fx::gltf::detail::WriteField( "spot", json, lightPunctual.spot );
    }
    fx::gltf::detail::WriteExtensions( json, lightPunctual.extensionsAndExtras );
}

struct gltf_KHR_lights_punctual {
    /// The vector of lights
    std::vector<gltf_lightPunctual> lights {};
    /// extensions and extra
    nlohmann::json extensionsAndExtras {};
};

inline void from_json( nlohmann::json const& json, gltf_KHR_lights_punctual& lights_punctual ) {
    fx::gltf::detail::ReadRequiredField( "lights", json, lights_punctual.lights );
    fx::gltf::detail::ReadExtensionsAndExtras( json, lights_punctual.extensionsAndExtras );
    if ( lights_punctual.lights.empty() ) {
        throw fx::gltf::invalid_gltf_document( "KHR_lights_punctual must have a least 1 light!" );
    }
}

inline void to_json( nlohmann::json& json, gltf_KHR_lights_punctual const& lights_punctual ) {
    fx::gltf::detail::WriteField( "lights", json, lights_punctual.lights );
    fx::gltf::detail::WriteExtensions( json, lights_punctual.extensionsAndExtras );
}

struct gltf_node_KHR_lights_punctual {
    /// The light index in the gltf_KHR_lights_punctual extension
    int32_t light { -1 };
    /// extensions and extra
    nlohmann::json extensionsAndExtras {};
};

inline void from_json( nlohmann::json const& json, gltf_node_KHR_lights_punctual& light ) {
    fx::gltf::detail::ReadRequiredField( "light", json, light.light );
    fx::gltf::detail::ReadExtensionsAndExtras( json, light.extensionsAndExtras );
}

inline void to_json( nlohmann::json& json, gltf_node_KHR_lights_punctual const& light ) {
    fx::gltf::detail::WriteField( "light", json, light.light, -1 );
    fx::gltf::detail::WriteExtensions( json, light.extensionsAndExtras );
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
