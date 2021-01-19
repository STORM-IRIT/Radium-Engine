#pragma once
#include <Core/RaCore.hpp>

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

/// Radium resources are locate in Resources directory, searched from Radium lib location.
/// @return the path to access Radium Resources. empty string if not found
/// @note the pattern searched is "Resrouces/Shaders" since it's the basic resources dir.
RA_CORE_API std::string getRadiumResourcesPath();

/// Radium plugins are locate in Plugins directory, searched from Radium lib location.
/// @return the path to access Radium Plugins. empty string if not found
/// @note the pattern searched is "Plugins/lib" since it's the basic resources dir.
RA_CORE_API std::string getRadiumPluginsPath();

/// @return the path prefix to access the current executable.
RA_CORE_API std::string getBasePath();

/// Search for an accesible Resources (or pattern if given) directory in the current executable (or
/// symbol if != nullptr) path or its parents.
/// @return the pattern path of the dynamic library or exec that contains the given symbol, empty
/// string if not found.
RA_CORE_API std::string getResourcesPath( void* symbol               = nullptr,
                                          const std::string& pattern = "Resources" );

} // namespace Resources
} // namespace Core
} // namespace Ra
