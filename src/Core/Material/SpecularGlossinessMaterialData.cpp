#include <Core/Material/SpecularGlossinessMaterialData.hpp>
namespace Ra {
namespace Core {
namespace Material {

SpecularGlossinessData::SpecularGlossinessData( const std::string& name ) :
    BaseGLTFMaterial( { "SpecularGlossiness" }, name ) {
    // extension supported by SpecularGlossiness gltf materials
}

} // namespace Material
} // namespace Core
} // namespace Ra
