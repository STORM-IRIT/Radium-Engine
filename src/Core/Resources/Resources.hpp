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
/// Get the path prefix to access Radium resources in a filesystem.
/// This prefix is the parent directory of the directory containing libRadiumCore, appended
/// with /Resources/ as all Radium resources will be installed, bundled or linked for the buildtree
/// in such a directory
RA_CORE_API std::string getRadiumResourcesPath();

/// Get the path to access Radium plugins in a filesystem.
/// This base prefix is the parent directory of the directory containing
/// libRadiumCore, appended with /Plugins/lib as all Radium plugins will be installed, bundled or
/// linked for the buildtree in such a directory
RA_CORE_API std::string getRadiumPluginsPath();

/// Get the path prefix to access the current executable.
RA_CORE_API std::string getBasePath();

/// Get the resource path of the current executable.
RA_CORE_API std::string getBaseResourcesPath();

/// Get a resource path that offsets the path to the dynamic library that contains
/// the given symbol by the given offset.
RA_CORE_API std::string getResourcesPath( void* symbol, const std::string& offset = "../" );

/// Get a resource path that corresponds to the search of the given pattern starting from the
/// offset fromt the path to the dynamic library or executable that contains the given symbol.
RA_CORE_API std::string
getResourcesPath( void* symbol, const std::string& pattern, const std::string& offset );

} // namespace Resources
} // namespace Core
} // namespace Ra
