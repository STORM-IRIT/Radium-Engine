#ifndef RADIUMENGINE_MATERIALCONVERTERS_HPP
#define RADIUMENGINE_MATERIALCONVERTERS_HPP

#include <Engine/RaEngine.hpp>

#include <functional>
#include <string>

namespace Ra {
namespace Core {
namespace Asset {
class MaterialData;
} // namespace Asset
} // namespace Core
namespace Engine {
class Material;

/**
 * A material converter is a couple <std::string,
 * std::function<Ra::Engine::Material*(Ra::Core::Asset::MaterialData*)>.
 * The string gives the name of the material, the function is whatever is
 * compatible with std::function:
 *  - a lambda
 *  - a functor
 *  - a function with bind parameters ...
 *
 * The function is in charge of converting a concrete Ra::Core::Asset::MaterialData*
 * to a concrete Ra::Engine::Material* according to the type of Material described
 * by the string.
 * \see Documentation on materials for instructions on how to extend the material system.
 */
namespace EngineMaterialConverters {

/// \name Convenience typedefs
/// \{
using AssetMaterialPtr = const Ra::Core::Asset::MaterialData*;
using RadiumMaterialPtr = Ra::Engine::Material*;
using ConverterFunction = std::function<RadiumMaterialPtr( AssetMaterialPtr )>;
/// \}

/**
 * Register a new Material converter.
 * \return true if the converter has been added, false otherwise
 *         (e.g.\ a converter with the same name exists).
 */
RA_ENGINE_API bool registerMaterialConverter( const std::string& name,
                                              ConverterFunction converter );

/**
 * Remove a Material converter.
 * \return true if the converter has been removed, false otherwise
 *         (e.g.\ a converter with the given name does't exist).
 */
RA_ENGINE_API bool removeMaterialConverter( const std::string& name );

/**
 * Look for a Material converter with a given name.
 * \return a pair containing:
 *          - true if a Material converter has been found, false otherwise.
 *          - the found Material converter if it has been found.
 */
// FIXME: just for the doc: what happens if the material converter cannot be found?
RA_ENGINE_API std::pair<bool, ConverterFunction> getMaterialConverter( const std::string& name );

} // namespace EngineMaterialConverters
} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MATERIALCONVERTERS_HPP
