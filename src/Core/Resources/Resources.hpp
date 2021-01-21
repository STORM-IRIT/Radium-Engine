#pragma once
#include <Core/RaCore.hpp>
#include <Core/Utils/StdOptional.hpp>

namespace Ra {
namespace Core {

/** Resources management for Radium.
 *  This namespace contains functions, classes and utilities for resource management.
 *  Resources are :
 *    - configuration file (ui, keymapping, ...)
 *    - default shaders
 *    - ...
 *
 *   @todo ensure that resources are installed or linked in the parent directory of the
 * libRadiumCore dynamic library.
 *
 *   @todo Generalize the locator such that one can find an offsetted directory from a dynamic
 *   library containing an arbitrary symbol. (easy with cpplocate)
 */
namespace Resources {
using namespace Ra::Core::Utils;
/// Radium resources are located in the Resources directory, searched from Radium lib location.
/// @return the path to access Radium Resources if found, otherwise !has_value
/// @note the pattern searched is "Resources/Shaders" since it's the basic resources dir.
RA_CORE_API optional<std::string> getRadiumResourcesPath();

/// Radium plugins are located in the Plugins directory, searched from Radium lib location.
/// @return the path to access Radium Plugins. empty string if not found
/// @note the pattern searched is "Plugins/lib" since it's the basic resources dir.
RA_CORE_API optional<std::string> getRadiumPluginsPath();

/// @return the path prefix to access the current executable (always found)
RA_CORE_API optional<std::string> getBasePath();

/// Search for an accessible Resources (or pattern if given) directory in the current executable (or
/// symbol if != nullptr) path or its parents.
/// @return the pattern path of the dynamic library or exec that contains the given symbol if found,
/// otherwise !has_value
RA_CORE_API optional<std::string> getResourcesPath( void* symbol               = nullptr,
                                                    const std::string& pattern = "Resources" );

} // namespace Resources
} // namespace Core
} // namespace Ra
