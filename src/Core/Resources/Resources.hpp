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
 *    @todo define and implement a portable interface for resource management.
 */
namespace Resources {
/// Get the base prefix to access the Radium resources in a filesystem.
RA_CORE_API const char* getBaseDir();
}; // namespace Resources
} // namespace Core
} // namespace Ra
#endif // RADIUMENGINE_RESOURCES_HPP
