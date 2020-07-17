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
/// This base prefix is the parent directory of the directory containing libRadiumCore, appended
/// with /Resources/ as all Radium resources will be installed, bundled or linked for the buildtree
/// in such a directory
RA_CORE_API std::string getRadiumResourcesDir();

/// Get the path prefix to access Radium plugins in a filesystem.
/// A Radium plugin is a plugin installed in the Radium bundled and embedded in installed
/// application bundle./// This base prefix is the parent directory of the directory containing
/// libRadiumCore, appended with /Plugins/lib as all Radium plugins will be installed, bundled or
/// linked for the buildtree in such a directory
RA_CORE_API std::string getRadiumPluginsDir();

/// Get the path prefix to access the current executable in a filesystem.
RA_CORE_API std::string getBaseDir();
RA_CORE_API std::string getBaseResourcesDir();

/// allow to manage several resource locators
class RA_CORE_API ResourcesLocator
{
  public:
    /// Construct a resource locator that will offset the path to the dynamic library that contains
    /// the given symbol by the given offset.
    explicit ResourcesLocator( void* symbol, const std::string& offset = "../" );
    /// Construct a resource locator that will search for the given pattern starting from the offset
    /// the path to the dynamic library or executable that contains the given symbol.
    explicit ResourcesLocator( void* symbol, std::string pattern, const std::string& offset );
    /// Return the base path corresponding of the locator execution
    const std::string& getBasePath();

  private:
    std::string m_basePath;
};
} // namespace Resources
} // namespace Core
} // namespace Ra
