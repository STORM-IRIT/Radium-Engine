#include <Core/Material/MetallicRoughnessMaterialData.hpp>

namespace Ra {
namespace Core {
namespace Material {

MetallicRoughnessData::MetallicRoughnessData( const std::string& name ) :
    BaseGLTFMaterial( { "MetallicRoughness" }, name ) {
    // extension supported by MetallicRoughness gltf materials
    allowExtensionList( { "KHR_materials_clearcoat",
                          "KHR_materials_ior",
                          "KHR_materials_specular",
                          "KHR_materials_sheen" } );

    // TODO : uncomment the extension when supported by the implementation.
    /*
        allowExtension("KHR_materials_transmission");
        allowExtension("KHR_materials_volume");
    */
}

} // namespace Material
} // namespace Core
} // namespace Ra
