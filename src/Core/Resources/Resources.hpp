#ifndef RADIUMENGINE_RESOURCES_HPP
#define RADIUMENGINE_RESOURCES_HPP
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
RA_CORE_API std::string getResourcesDir();

/// Get the path prefix to access Radium plugins in a filesystem.
/// This base prefix is the parent directory of the directory containing libRadiumCore, appended
/// with /Plugins/lib as all Radium plugins will be installed, bundled or linked for the buildtree
/// in such a directory
RA_CORE_API std::string getPluginsDir();

/// Get the path prefix to access the current executable in a filesystem.
RA_CORE_API std::string getBaseDir();

/// allow to manage several resource locators
RA_CORE_API class ResourcesLocator
{
  public:
    /// Construct a resource locator tha will offset the path to the dynamic library that contains
    /// the given symbol by the fgiven offset.
    explicit ResourcesLocator( void* symbol, const std::string& offset = "../" );
    const std::string& getBasePath();

  private:
    std::string m_basePath;
};
} // namespace Resources
} // namespace Core
} // namespace Ra
#endif // RADIUMENGINE_RESOURCES_HPP
