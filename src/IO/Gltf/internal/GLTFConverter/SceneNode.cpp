#include <IO/Gltf/internal/GLTFConverter/SceneNode.hpp>

#include <IO/Gltf/internal/Extensions/LightExtensions.hpp>

namespace Ra {
namespace IO {
namespace GLTF {

void SceneNode::initPropsFromExtensionsAndExtra( const nlohmann::json& extensionsAndExtras ) {
    // manage node extension
    if ( !extensionsAndExtras.empty() ) {
        auto extensions = extensionsAndExtras.find( "extensions" );
        if ( extensions != extensionsAndExtras.end() ) {
            auto iter = extensions->find( "KHR_lights_punctual" );
            if ( iter != extensions->end() ) {
                gltf_node_KHR_lights_punctual light;
                from_json( *iter, light );
                // TODO do wee need more than that ?
                //  Do we need to keep the full extension definition (gltf_node_KHR_lights_punctual)
                //  ?
                this->m_lightIndex = light.light;
            }
        }
    }
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
