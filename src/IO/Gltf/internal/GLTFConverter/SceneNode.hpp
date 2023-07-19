#pragma once
#include <IO/Gltf/internal/fx/gltf.h>

#include <Core/Types.hpp>

namespace Ra {
namespace IO {
namespace GLTF {

/// TODO : make graph node more adapted
struct SceneNode {
    Ra::Core::Transform m_transform;
    int32_t m_cameraIndex { -1 };
    int32_t m_meshIndex { -1 };
    int32_t m_skinIndex { -1 };
    std::string m_nodeName;
    /// only used with KHR_lights_punctual extension
    int32_t m_lightIndex { -1 };
    std::vector<int32_t> children {};

    void initPropsFromExtensionsAndExtra( const nlohmann::json& extensionsAndExtras );
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
