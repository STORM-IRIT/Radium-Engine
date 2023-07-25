#include <Core/Material/BaseGLTFMaterial.hpp>
namespace Ra {
namespace Core {
namespace Material {

BaseGLTFMaterial::BaseGLTFMaterial( const std::string& gltfType, const std::string& instanceName ) :
    Ra::Core::Asset::MaterialData( instanceName, gltfType ) {
    // extension supported by all gltf materials
    allowExtension( "KHR_materials_unlit" );
}

} // namespace Material
} // namespace Core
} // namespace Ra
