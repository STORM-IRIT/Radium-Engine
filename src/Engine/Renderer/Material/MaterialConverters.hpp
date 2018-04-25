#ifndef RADIUMENGINE_MATERIALCONVERTERS_HPP
#define RADIUMENGINE_MATERIALCONVERTERS_HPP

#include <Engine/RaEngine.hpp>

#include <functional>
#include <string>

namespace Ra {
namespace Engine {
class Material;
}
namespace Asset {
class MaterialData;
}
} // namespace Ra

/// A material converter is a couple <std::string,
/// std::function<Ra::Engine::Material*(Ra::Asset::MaterialData*)> The string gives the mname of the
/// material, the function is whatever is compatible with std::function :
///     - a lambda
///     - a functor
///     - a function with bind parameters ....
/// The function is in charge of converting a concrete Ra::Asset::MaterialData* to a concrete
/// Ra::Engine::Material* according to the type of material described by the string ...
/**
 * Instruction on how to extend the material system
 */
///////////////////////////////////////////////
////        Material converter system       ///
///////////////////////////////////////////////

namespace Ra {
namespace Engine {
///////////////////////////////////////////////
////        Radium Material converters      ///
///////////////////////////////////////////////

class RA_ENGINE_API MaterialConverter final {
  public:
    MaterialConverter() = default;
    ~MaterialConverter() = default;

    Material* operator()( const Ra::Asset::MaterialData* toconvert );
};

namespace EngineMaterialConverters {

using AssetMaterialPtr = const Ra::Asset::MaterialData*;
using RadiumMaterialPtr = Ra::Engine::Material*;
using ConverterFunction = std::function<RadiumMaterialPtr( AssetMaterialPtr )>;

/** register a new material converter
 *  @return true if converter added, false else (e.g, a converter with the same name exists)
 */
RA_ENGINE_API bool registerMaterialConverter( const std::string& name,
                                              ConverterFunction converter );

/** remove a material converter
 *  @return true if converter removed, false else (e.g, a converter with the same name does't
 * exists)
 */
RA_ENGINE_API bool removeMaterialConverter( const std::string& name );

/**
 * @param name name of the material to convert
 * @return a pair containing the search result and, if true, the functor to call to convert the
 * material
 */
RA_ENGINE_API std::pair<bool, ConverterFunction> getMaterialConverter( const std::string& name );

} // namespace EngineMaterialConverters
} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MATERIALCONVERTERS_HPP
